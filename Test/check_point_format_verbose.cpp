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
            out += "'\"'\"'";
        else
            out += c;
    }
    out += "'";
    return out;
}

// Run command and capture stdout+stderr and return pair(output, exit_code)
static std::pair<std::string,int> run_cmd_with_exit(const std::string &cmd)
{
    std::array<char, 4096> buf;
    std::string result;
    // capture stderr as well
    std::string fullcmd = cmd + " 2>&1";
    FILE *pipe = popen(fullcmd.c_str(), "r");
    if (!pipe) return {std::string(), -1};
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe) != nullptr)
    {
        result += buf.data();
    }
    int rc = pclose(pipe);
    int exit_code = WEXITSTATUS(rc);
    return {result, exit_code};
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Użycie: " << argv[0] << " \"Chmura Zadanie.las\"\n";
        return 1;
    }

    std::string filename = argv[1];

    // show pdal path and version
    auto [ver_out, ver_code] = run_cmd_with_exit("which pdal && pdal --version");
    std::cout << "pdal info (path & version):\n" << ver_out << "\n";

    std::string cmd = "pdal info --metadata " + shell_escape_single(filename);
    std::cout << "Uruchamiam: " << cmd << "\n\n";

    auto [out, code] = run_cmd_with_exit(cmd);

    std::cout << "Kod powrotu pdal: " << code << "\n";
    std::cout << "Wyjście pdal (stdout+stderr):\n";
    if (out.empty())
        std::cout << "(brak wyjścia)\n";
    else
        std::cout << out << "\n";

    // jeśli pdal nie zwrócił metadanych, spróbujmy wypisać nagłówek pliku
    if (code != 0 || out.find("metadata") == std::string::npos)
    {
        std::cout << "\nDodatkowa diagnostyka pliku:\n";
        std::string esc = shell_escape_single(filename);
        auto [ls_out, ls_code] = run_cmd_with_exit(std::string("ls -l ") + esc);
        auto [file_out, file_code] = run_cmd_with_exit(std::string("file -b --mime-type ") + esc);
        auto [head_out, head_code] = run_cmd_with_exit(std::string("head -c 16 ") + esc + " | hexdump -C");

        std::cout << "ls -l:\n" << ls_out << "\n";
        std::cout << "file (MIME type):\n" << file_out << "\n";
        std::cout << "pierwsze 16 bajtów (hexdump):\n" << head_out << "\n";
    }

    return (code == 0) ? 0 : 2;
}