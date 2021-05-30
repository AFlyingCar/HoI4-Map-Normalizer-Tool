
#ifndef TEST_OVERRIDES_H
# define TEST_OVERRIDES_H

# include "Options.h"

/**
 * @brief Sets a program option value for the scope it is declared in
 *
 * @tparam T The type of the option value to set
 */
template<typename T>
class ProgramOptionSetter {
    public:
        ProgramOptionSetter(T* option_address, T&& value):
            m_option_address(option_address),
            m_old_value(*option_address)
        {
            *m_option_address = value;
        }

        ~ProgramOptionSetter() {
            *m_option_address = m_old_value;
        }

    private:
        T* m_option_address;
        T m_old_value;
};

# define PRIM_CONCAT(A,B) A##B
# define CONCAT(A,B) PRIM_CONCAT(A, B)
# define SET_PROGRAM_OPTION(OPTION_NAME, VALUE) \
    ProgramOptionSetter<decltype(MapNormalizer::prog_opts. OPTION_NAME)> CONCAT(_PROGRAM_OPTION_SETTER_,__LINE__) (&MapNormalizer::prog_opts. OPTION_NAME, (VALUE))

#endif
