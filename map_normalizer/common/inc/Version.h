#ifndef VERSION_H
# define VERSION_H

# include <string>
# include <ostream>

namespace MapNormalizer {
    class Version {
        public:
            Version();
            Version(const Version&);
            Version(Version&&);

            Version(const std::string&);

            ~Version() = default;

            Version& operator=(const Version&);

            bool operator!=(const Version&) const;
            bool operator==(const Version&) const;
            bool operator<(const Version&) const;
            bool operator>(const Version&) const;
            bool operator<=(const Version&) const;
            bool operator>=(const Version&) const;

            const std::string& str() const;

        protected:
            int compare(const Version&) const;

        private:
            std::string m_version;

            friend std::ostream& operator<<(std::ostream&, const Version&);
    };

    std::ostream& operator<<(std::ostream&, const Version&);
}

#endif
