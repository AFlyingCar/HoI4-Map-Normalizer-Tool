
#include "SelectionManager.h"

#include <algorithm>

#include "Driver.h"
#include "Constants.h"

auto HMDT::GUI::SelectionManager::getInstance() -> SelectionManager& {
    static SelectionManager instance;

    return instance;
}

void HMDT::GUI::SelectionManager::selectProvince(const ProvinceID& label,
                                                 bool skip_callback,
                                                 OptCallbackData data)
{
    // Do not select if the label isn't valid
    if(auto opt_mproj = getCurrentMapProject();
            opt_mproj && opt_mproj->get().getProvinceProject().isValidProvinceID(label))
    {
        if(!skip_callback) {
            m_on_province_selected_callback(label, Action::SET, data);
        }
        m_selected_provinces = {label};
    }
}

void HMDT::GUI::SelectionManager::addProvinceSelection(const ProvinceID& label,
                                                       bool skip_callback,
                                                       OptCallbackData data)
{
    // Do not select if the label isn't valid
    if(auto opt_mproj = getCurrentMapProject();
            opt_mproj && opt_mproj->get().getProvinceProject().isValidProvinceID(label))
    {
        if(!skip_callback) {
            m_on_province_selected_callback(label, Action::ADD, data);
        }
        m_selected_provinces.insert(label);
    }
}

void HMDT::GUI::SelectionManager::removeProvinceSelection(const ProvinceID& label,
                                                          bool skip_callback,
                                                          OptCallbackData data)
{
    if(!skip_callback) {
        m_on_province_selected_callback(label, Action::REMOVE, data);
    }
    m_selected_provinces.erase(label);
}

void HMDT::GUI::SelectionManager::clearProvinceSelection(bool skip_callback,
                                                         OptCallbackData data)
{
    if(!skip_callback) {
        m_on_province_selected_callback(INVALID_PROVINCE, Action::CLEAR, data);
    }
    m_selected_provinces.clear();
}

void HMDT::GUI::SelectionManager::selectState(StateID state_id) {
    // Do not select if the state_id isn't valid
    if(auto opt_hproj = getCurrentHistoryProject();
            opt_hproj && opt_hproj->get().getStateProject().isValidStateID(state_id))
    {
        m_on_state_selected_callback(state_id, Action::SET);
        m_selected_states = {state_id};
    }
}

void HMDT::GUI::SelectionManager::addStateSelection(StateID state_id) {
    // Do not select if the state_id isn't valid
    if(auto opt_hproj = getCurrentHistoryProject();
            opt_hproj && opt_hproj->get().getStateProject().isValidStateID(state_id))
    {
        m_on_state_selected_callback(state_id, Action::ADD);
        m_selected_states.insert(state_id);
    }
}

void HMDT::GUI::SelectionManager::removeStateSelection(StateID state_id) {
    m_on_state_selected_callback(state_id, Action::REMOVE);
    m_selected_states.erase(state_id);
}

void HMDT::GUI::SelectionManager::clearStateSelection() {
    m_on_state_selected_callback(INVALID_STATE_ID, Action::CLEAR);
    m_selected_states.clear();
}

void HMDT::GUI::SelectionManager::setOnSelectProvinceCallback(const OnSelectProvinceCallback& on_province_selected_callback)
{
    m_on_province_selected_callback = on_province_selected_callback;
}

void HMDT::GUI::SelectionManager::setOnSelectStateCallback(const OnSelectStateCallback& on_state_selected_callback)
{
    m_on_state_selected_callback = on_state_selected_callback;
}

size_t HMDT::GUI::SelectionManager::getSelectedProvinceCount() const {
    return m_selected_provinces.size();
}

size_t HMDT::GUI::SelectionManager::getSelectedStateCount() const {
    return m_selected_states.size();
}

/**
 * @brief Will return the currently selected provinces.
 *
 * @return The currently selected provinces.
 */
auto HMDT::GUI::SelectionManager::getSelectedProvinces() const
    -> RefVector<const Province>
{
    RefVector<const Province> provinces;

    if(auto opt_mproj = getCurrentMapProject(); opt_mproj)
    {
        auto& mproj = opt_mproj->get();
        std::transform(m_selected_provinces.begin(), m_selected_provinces.end(),
                       std::back_inserter(provinces),
                       [&mproj](const ProvinceID& prov_id) {
                           return std::ref(mproj.getProvinceProject().getProvinceForID(prov_id));
                       });
    }

    return provinces;
}

/**
 * @brief Will return the currently selected provinces.
 *
 * @return The currently selected provinces.
 */
auto HMDT::GUI::SelectionManager::getSelectedProvinces() -> RefVector<Province>
{
    RefVector<Province> provinces;
    if(auto opt_mproj = getCurrentMapProject(); opt_mproj)
    {
        auto& mproj = opt_mproj->get();
        std::transform(m_selected_provinces.begin(), m_selected_provinces.end(),
                       std::back_inserter(provinces),
                       [&mproj](const ProvinceID& prov_id) {
                           return std::ref(mproj.getProvinceProject().getProvinceForID(prov_id));
                       });
    }
    return provinces;
}

auto HMDT::GUI::SelectionManager::getSelectedProvinceLabels() const
    -> const std::set<ProvinceID>&
{
    return m_selected_provinces;
}

/**
 * @brief Will return the currently selected states.
 *
 * @return The currently selected states.
 */
auto HMDT::GUI::SelectionManager::getSelectedStates() const
    -> RefVector<const State>
{
    RefVector<const State> states;
    if(auto opt_hproj = getCurrentHistoryProject(); opt_hproj)
    {
        auto& hproj = opt_hproj->get();
        std::transform(m_selected_states.begin(), m_selected_states.end(),
                       std::back_inserter(states),
                       [&hproj](StateID state_id) -> const State& {
                           return hproj.getStateProject().getStateForID(state_id)->get();
                       });
    }
    return states;
}

/**
 * @brief Will return the currently selected states.
 *
 * @return The currently selected states.
 */
auto HMDT::GUI::SelectionManager::getSelectedStates() -> RefVector<State> {
    RefVector<State> states;
    if(auto opt_hproj = getCurrentHistoryProject(); opt_hproj)
    {
        auto& hproj = opt_hproj->get();
        std::transform(m_selected_states.begin(), m_selected_states.end(),
                       std::back_inserter(states),
                       [&hproj](StateID state_id) -> State& {
                           return hproj.getStateProject().getStateForID(state_id)->get();
                       });
    }
    return states;
}

auto HMDT::GUI::SelectionManager::getSelectedStateIDs() const
    -> const std::set<uint32_t>&
{
    return m_selected_states;
}

/**
 * @brief Checks if the given province ID is currently selected
 *
 * @param id The province ID to check
 */
bool HMDT::GUI::SelectionManager::isProvinceSelected(const ProvinceID& id) const
{
    return m_selected_provinces.count(id) != 0;
}

/**
 * @brief Checks if the given state ID is currently selected
 *
 * @param id The state ID to check
 */
bool HMDT::GUI::SelectionManager::isStateSelected(const StateID& id) const {
    return m_selected_states.count(id) != 0;
}

/**
 * @brief Clears out all selection information
 */
void HMDT::GUI::SelectionManager::onProjectUnloaded() {
    m_selected_provinces.clear();
    m_selected_states.clear();
}

HMDT::GUI::SelectionManager::SelectionManager():
    m_selected_provinces(),
    m_selected_states(),
    m_on_province_selected_callback([](auto...) { }),
    m_on_state_selected_callback([](auto...) { })
{ }

auto HMDT::GUI::SelectionManager::getCurrentMapProject() const
    -> OptionalReference<Project::IRootMapProject>
{
    if(auto opt_project = Driver::getInstance().getProject(); opt_project) {
        return std::ref(opt_project->get().getMapProject());
    } else {
        return std::nullopt;
    }
}

auto HMDT::GUI::SelectionManager::getCurrentHistoryProject() const
    -> OptionalReference<Project::IRootHistoryProject>
{
    if(auto opt_project = Driver::getInstance().getProject(); opt_project) {
        return std::ref(opt_project->get().getHistoryProject());
    } else {
        return std::nullopt;
    }
}

