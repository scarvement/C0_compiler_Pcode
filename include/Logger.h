#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <sstream>
#include <ostream>

class Logger {
   public:
        bool hasFatal;
/*
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }
        Logger(Logger const&) = delete;
        void operator=(Logger const&) = delete;

        void warn(const SourceCode::const_iterator &pos, const std::string &prompt) {
            echo("Warning", pos, prompt);
        }
        void error(const SourceCode::const_iterator &pos, const std::string &prompt) {
            echo("Error", pos, prompt);
        }
        void fatal(const SourceCode::const_iterator &pos, const std::string &prompt) {
            hasFatal = true;
            echo("Fatal", pos, prompt);
        }

        void output(std::ostream &stream) {
            stream << ss.rdbuf();
            ss.clear();
        }
    private:
        std::stringstream ss;

        Logger(): hasFatal(false) {}

        void echo(const std::string &prefix, const SourceCode::const_iterator &pos, const std::string &prompt);
*/
};

#endif // LOGGER_H
