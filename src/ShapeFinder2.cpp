
#include "ShapeFinder2.h"

#include <sstream>

#include "Logger.h"
#include "Util.h"
#include "Constants.h"
#include "ProvinceMapBuilder.h" // getProvinceType
#include "UniqueColorGenerator.h" // generateUniqueColor
#include "Options.h"

using namespace std::string_literals;

uint32_t getRootLabel(uint32_t label,
                      const std::map<uint32_t, uint32_t>& label_parents)
{
    uint32_t root = label;

    while(label_parents.count(root) != 0) {
        root = label_parents.at(root);
    }

    return root;
}

/**
 * @brief Gets the label and the color for the given point.
 *
 * @param image The image to get the color from.
 * @param point The point to get the color and label for.
 * @param label_matrix The matrix to get the labels from
 * @param color The current color to compare the gotten color against
 *
 * @return A pair containing both the label and the color
 */
auto MapNormalizer::getLabelAndColor(BitMap* image, const Point2D& point,
                                     uint32_t* label_matrix, const Color& color)
    -> std::pair<uint32_t, Color>
{
    uint32_t label = label_matrix[xyToIndex(image, point.x, point.y)];
    Color color_at = getColorAt(image, point.x, point.y);

    if(color_at != BORDER_COLOR && color_at != color) {
        writeWarning("Multiple colors found in shape! See pixel at "s + point);

        // Set to the default values
        label = 0;
        color_at = BORDER_COLOR;
    }

    return {label, color_at};
}

/**
 * @brief Gets a pixel adjacent to point
 *
 * @param image The image the point is from
 * @param point The point to get an adjacent pixel for.
 * @param dir1 The first direction. Cannot be NONE
 * @param dir2 The second direction. Cannot be along the same axis
 *             (LEFT->RIGHT, UP->DOWN) as dir1
 *
 * @return The point adjacent to 'point', std::nullopt if there is no pixel
 *         adjacent to 'point' in the directions specified
 */
auto MapNormalizer::getAdjacentPixel(BitMap* image, Point2D point,
                                     Direction dir1, Direction dir2)
    -> std::optional<Point2D>
{
    Point2D adjacent = point;

    // If the directions are along the same axis then we have bad inputs
    if(dir2 != Direction::NONE && ((int)dir1 % 2 == (int)dir2 % 2)) {
        writeWarning("Invalid input to getAdjacentPixel! dir1 cannot be along the same axis as dir2!");

        return std::nullopt;
    }

    switch(dir1) {
        case Direction::LEFT:
            adjacent.x = point.x - 1;
            break;
        case Direction::RIGHT:
            adjacent.x = point.x + 1;
            break;
        case Direction::UP:
            adjacent.y = point.y - 1;
            break;
        case Direction::DOWN:
            adjacent.y = point.y + 1;
            break;
        case Direction::NONE:
            writeWarning("Invalid input to getAdjacentPixel! dir1 cannot be NONE");

            return std::nullopt;
    }

    switch(dir1) {
        case Direction::LEFT:
            adjacent.x = point.x - 1;
            break;
        case Direction::RIGHT:
            adjacent.x = point.x + 1;
            break;
        case Direction::UP:
            adjacent.y = point.y - 1;
            break;
        case Direction::DOWN:
            adjacent.y = point.y + 1;
            break;
        default:;
    }

    if(isInImage(image, adjacent.x, adjacent.y) &&
       getColorAt(image, adjacent.x, adjacent.y) != BORDER_COLOR)
    {
        return adjacent;
    } else {
        return std::nullopt;
    }
}

void MapNormalizer::CCLPass1(BitMap* image, uint32_t* label_matrix,
                             std::map<uint32_t, uint32_t>& label_parents)
{
    uint32_t width = image->info_header.width;
    uint32_t height = image->info_header.height;

    uint32_t next_label = 1;

    if(!MapNormalizer::prog_opts.quiet)
        writeStdout("Performing Pass #1 of CCL.");

    for(uint32_t y = 0; y < height; ++y) {
        for(uint32_t x = 0; x < width; ++x) {
            Color color = getColorAt(image, x, y);
            uint32_t index = xyToIndex(image, x, y);
            uint32_t& label = label_matrix[index] = next_label;

            if(!MapNormalizer::prog_opts.quiet)
                setInfoLine("Pixel "s + Point2D{x, y} + " [" + color + "]");

            // Skip this pixel if it is part of a border
            if(color == BORDER_COLOR) {
                label_matrix[index] = 0; // Reset the label back to 0
                continue;
            }

            // std::nullopt => not in image, treat as border
            std::optional<Point2D> left = getAdjacentPixel(image, Point2D{x, y},
                                                           Direction::LEFT);
            std::optional<Point2D> up = getAdjacentPixel(image, Point2D{x, y},
                                                         Direction::UP);
#if 0
            std::optional<Point2D> up_left = getAdjacentPixel(image,
                                                              Point2D{x, y},
                                                              Direction::UP,
                                                              Direction::LEFT);
#endif

            uint32_t label_left = 0;
            uint32_t label_up = 0;
            // uint32_t label_up_left = 0;

            Color color_left = BORDER_COLOR;
            Color color_up = BORDER_COLOR;
            // Color color_up_left = BORDER_COLOR;

            // Get the label and color of adjacent pixels that we have already
            //  visited
            if(left) {
                std::tie(label_left, color_left) = getLabelAndColor(image, *left, label_matrix, color);
            }

            if(up) {
                std::tie(label_up, color_up) = getLabelAndColor(image, *up, label_matrix, color);
            }

#if 0
            if(up_left) {
                std::tie(label_up_left, color_up_left) = getLabelAndColor(image, *up_left, label_matrix, color);
            }
#endif

            // Compare the color of the adjacent pixels to ourself
            // getLabelAndColor will auto-convert all colors to BORDER_COLOR if
            //  the color does not match the current one, so there is no need
            //  to check for that here
            if(color_left != BORDER_COLOR) {
                label = label_left;
            }

            if(color_up != BORDER_COLOR) {
                // If we have already chosen an adjacent label
                if(label != next_label) {
                    // If the adjacent label does not match, then pick the
                    //   smaller one and mark the larger one as a child
                    if(label != label_up) {
                        uint32_t smaller = std::min(label, label_up);
                        uint32_t larger = std::max(label, label_up);

                        label = smaller;
                        // Mark who the parent of the label is
                        // TODO: Do we have to worry about if the label already has a parent?
                        label_parents[larger] = smaller;
                    }
                } else {
                    label = label_up;
                }
            }

#if 0
            if(color_up_left != BORDER_COLOR) {
                // If we have already chosen an adjacent label
                if(label != next_label) {
                    // If the adjacent label does not match, then pick the
                    //   smaller one and mark the larger one as a child
                    if(label != label_up_left) {
                        uint32_t smaller = std::min(label, label_up_left);
                        uint32_t larger = std::max(label, label_up_left);

                        label = smaller;
                        // Mark who the parent of the label is
                        // TODO: Do we have to worry about if the label already has a parent?
                        label_parents[larger] = smaller;
                    }
                } else {
                    label = label_up_left;
                }
            }
#endif

            if(MapNormalizer::prog_opts.verbose)
                writeDebug("Pixel "s + Point2D{x, y} + " [" + color + "] => " + std::to_string(label),
                           false);

            // Only increment to the next label if we actually used this one
            if(label == next_label) {
                ++next_label;
            }
        }
    }

    if(!MapNormalizer::prog_opts.quiet)
        setInfoLine("");
}

void MapNormalizer::CCLPass2(BitMap* image, uint32_t* label_matrix,
                             const std::map<uint32_t, uint32_t>& label_parents)
{
    uint32_t width = image->info_header.width;
    uint32_t height = image->info_header.height;

    if(!MapNormalizer::prog_opts.quiet)
        writeStdout("Performing Pass #2 of CCL.");

    for(uint32_t y = 0; y < height; ++y) {
        for(uint32_t x = 0; x < width; ++x) {
            uint32_t index = xyToIndex(image, x, y);
            uint32_t& label = label_matrix[index];

            // Will return itself if this label is already a root
            label = getRootLabel(label, label_parents);
        }
    }
}

MapNormalizer::PolygonList MapNormalizer::CCLPass3(BitMap* image,
                                                   uint32_t* label_matrix)
{
    uint32_t width = image->info_header.width;
    uint32_t height = image->info_header.height;

    PolygonList shapes;
    std::map<uint32_t, uint32_t> label_to_shapeidx;

    if(!MapNormalizer::prog_opts.quiet)
        writeStdout("Performing Pass #3 of CCL.");

    for(uint32_t y = 0; y < height; ++y) {
        for(uint32_t x = 0; x < width; ++x) {
            uint32_t index = xyToIndex(image, x, y);
            Color color = getColorAt(image, x, y);
            uint32_t label = label_matrix[index];
            Point2D point{x, y};

            if(color == BORDER_COLOR) {
                // If we are a border pixel, then merge with the closest shape
                // First, check anything to our upper-left, as those will be
                //  non-borders if they exist
                std::optional<Point2D> opt_adjacent;

                if(opt_adjacent = getAdjacentPixel(image, point,
                                                   Direction::LEFT);
                   opt_adjacent)
                {
                    index = xyToIndex(image, opt_adjacent->x, opt_adjacent->y);
                    color = getColorAt(image, opt_adjacent->x, opt_adjacent->y);
                    label = label_matrix[index];
                } else if(opt_adjacent = getAdjacentPixel(image, point,
                                                          Direction::UP);
                          opt_adjacent)
                {
                    index = xyToIndex(image, opt_adjacent->x, opt_adjacent->y);
                    color = getColorAt(image, opt_adjacent->x, opt_adjacent->y);
                    label = label_matrix[index];
#if 0
                } else if(opt_adjacent = getAdjacentPixel(image, point,
                                                          Direction::UP,
                                                          Direction::LEFT);
                          opt_adjacent)
                {
                    index = xyToIndex(image, opt_adjacent->x, opt_adjacent->y);
                    color = getColorAt(image, opt_adjacent->x, opt_adjacent->y);
                    label = label_matrix[index];
#endif
                } else {
                    // If that fails, start walking left->right, top->bottom for the
                    //  first pixel that is not a border and merge with that
                    for(uint32_t y2 = y; y2 < height; ++y2) {
                        for(uint32_t x2 = x; x2 < width; ++x2) {
                            index = xyToIndex(image, x2, y2);
                            color = getColorAt(image, x2, y2);
                            label = label_matrix[index];

                            // Skip any further border pixels, then will be
                            //  picked up by the earlier adjacency checks
                            if(color == BORDER_COLOR) {
                                continue;
                            }
                        }
                    }

                    // If we _still_ have a border color, then that means we are
                    //  in a worst case scenario of the entire image being
                    //  (0,0,0)
                    if(color == BORDER_COLOR) {
                        writeError("No further color pixels found from "s + point + ". Terminating now! Check your input image!");
                        return PolygonList{}; // Return 0 shapes to indicate an error
                    }
                }
            }

            uint32_t shapeidx = -1;

            // Do we have an entry for this label yet?
            if(label_to_shapeidx.count(label) == 0) {
                label_to_shapeidx[label] = shapes.size();

                // Create a new shape
                auto prov_type = getProvinceType(color);
                Color unique_color = generateUniqueColor(prov_type);

                shapes.push_back(Polygon{
                    { },
                    color,
                    unique_color,
                    { 0, 0 }, { 0, 0 }
                });
            }

            shapeidx = label_to_shapeidx[label];

            Polygon& shape = shapes.at(shapeidx);

            shape.pixels.push_back(Pixel{ point, color });

            // We calculate the bounding box of the shape incrementally
            if(point.x > shape.top_right.x) {
                shape.top_right.x = point.x;
            } else if(point.x < shape.bottom_left.x) {
                shape.bottom_left.x = point.x;
            }

            if(point.y > shape.top_right.y) {
                shape.top_right.y = point.y;
            } else if(point.y < shape.bottom_left.y) {
                shape.bottom_left.y = point.y;
            }
        }
    }

    if(!MapNormalizer::prog_opts.quiet)
        writeStdout("Generated "s + std::to_string(shapes.size()) + " shapes.");

    // Perform error-checking on shapes
    uint32_t index = 0;
    for(Polygon& shape : shapes) {
        ++index;

        // Check for minimum province size.
        //  See: https://hoi4.paradoxwikis.com/Map_modding
        if(shape.pixels.size() <= MIN_SHAPE_SIZE) {
            std::stringstream ss;
            ss << "Shape " << index << " has only " << shape.pixels.size()
               << " pixels. All provinces are required to have more than "
               << MIN_SHAPE_SIZE
               << " pixels. See: https://hoi4.paradoxwikis.com/Map_modding";
            writeWarning(ss.str());
        }

        //  Check to make sure bounding boxes aren't too large
        if(auto [width, height] = calcShapeDims(shape); isShapeTooLarge(width,
                                                                        height,
                                                                        image))
        {
            std::stringstream ss;
            ss << "Shape #" << index << " has a bounding box of size "
               << Point2D{width, height} << ". One of these is larger than the"
               << " allowed ratio of 1/8 * ("
               << image->info_header.width << ',' << image->info_header.height
               << ") => (" << (image->info_header.width / 8.0f) << ','
                          << (image->info_header.height / 8.0f)
               << "). Check the province borders.";
            ss << " Bounds are: " << shape.bottom_left << " to " << shape.top_right;
            writeWarning(ss.str());
        }
    }

    return shapes;
}

/**
 * @brief Finds all shapes in a given BitMap image 
 *
 * @param image The image to get all shapes from
 *
 * @return A list of all shapes in the BitMap image
 */
MapNormalizer::PolygonList MapNormalizer::findAllShapes2(BitMap* image)
{
    // Unlike findAllShapes, we will do a Connected-Component Labeling (CCL)
    //  algorithm here
    // We also do not accept arrays to fill in. Those will be filled in _after_
    //  this algorithm completes using only the PolygonList, which will make
    //  error-checking easier

    // Algorithm adapted from:
    //   https://www.aishack.in/tutorials/labelling-connected-components-example/
    // Pseudo-code to perform Connected Component Labeling in 3 passes
    //
    // First Pass:
    //   for every pixel (left->right, top->bottom):
    //     Check left pixel, up pixel, up-left pixel:
    //      Are any of them not a border pixel (0,0,0)
    //        Is the color the same as this pixel?
    //            use the same label
    //            If any of them disagree on labels, then choose the smaller one
    //             also, mark that '2' (the larger label) is a "child" of '1'
    //        else warn/error (put into the same shape?)
    //      If they are (0,0,0) pixels then skip and ignore
    // Second Pass:
    //   for every pixel [label] (left->right, top->bottom):
    //     If the label is a "child":
    //       Get the "root" label (child->parent->parent->...->parent->root)
    //       Replace label with "root"
    // Third Pass:
    //   Add all (0,0,0) pixels to their nearest shape, generate 'Polygon's for
    //    each label

    // Allocate one uint32_t for every pixel
    uint32_t label_matrix_size = image->info_header.width * image->info_header.height;
    uint32_t* label_matrix = new uint32_t[label_matrix_size];
    std::map<uint32_t, uint32_t> label_parents;

    CCLPass1(image, label_matrix, label_parents);
    CCLPass2(image, label_matrix, label_parents);
    return CCLPass3(image, label_matrix);
}
