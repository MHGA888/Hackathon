#include <cstdio>
#include <iostream>
#include <memory>
#include <array>
#include <string>
#include <regex>

// Escape filename for single-quoted shell argument
static std::string shell_escape_single(const std::string &s)
{
    std::string out = "'";
    for (char c : s)
    {
        if (c == '\'')
            out += "'\"'\"'"; // close, add escaped single quote, reopen
        else
            out += c;
    }
    out += "'";
    return out;
}

// Run command and capture stdout (returns empty string on error)
static std::string run_cmd(const std::string &cmd)
{
    std::array<char, 4096> buf;
    std::string result;
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) return {};
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe) != nullptr)
    {
        result += buf.data();
    }
    int rc = pclose(pipe);
    (void)rc;
    return result;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Użycie: " << argv[0] << " \"Chmura Zadanie.las\"\n";
        return 1;
    }

    std::string filename = argv[1];

    // Use pdal CLI to get metadata as JSON. Suppress stderr to avoid noise.
    std::string cmd = "pdal info --metadata " + shell_escape_single(filename) + " 2>/dev/null";
    std::string meta = run_cmd(cmd);

    if (meta.empty())
    {
        std::cerr << "Nie udało się uruchomić 'pdal info' lub nie zwrócono żadnych metadanych.\n";
        std::cerr << "Sprawdź, czy plik istnieje i czy 'pdal' jest w PATH.\n";
        return 2;
    }

    // Try several common JSON key names for point format
    const std::vector<std::regex> patterns = {
        std::regex(R"("point_format"\s*:\s*([0-9]+))", std::regex::icase),
        std::regex(R"("pointFormat"\s*:\s*([0-9]+))", std::regex::icase),
        std::regex(R"("point_format_id"\s*:\s*([0-9]+))", std::regex::icase),
        // sometimes inside readers.las object as "format": <num>
        std::regex(R"("format"\s*:\s*([0-9]+))", std::regex::icase)
    };

    std::smatch m;
    for (auto &re : patterns)
    {
        if (std::regex_search(meta, m, re) && m.size() >= 2)
        {
            std::cout << "Point Format: " << m[1].str() << "\n";
            return 0;
        }
    }

    std::cout << "Nie znaleziono informacji o Point Format w metadanych.\n";
    // Jeśli chcesz zobaczyć metadane dla diagnostyki, odkomentuj poniższe:
    // std::cout << meta << std::endl;
    return 3;
}