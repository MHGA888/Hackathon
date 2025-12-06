#include <cstdio>
#include <iostream>
#include <memory>
#include <array>
#include <string>
#include <vector>
#include <json/json.h>

// Uruchamia polecenie i zwraca stdout+stderr oraz kod powrotu
static std::pair<std::string,int> run_cmd(const std::string &cmd)
{
    std::array<char, 4096> buf;
    std::string out;
    std::string full = cmd + " 2>&1";
    FILE *pipe = popen(full.c_str(), "r");
    if (!pipe) return {std::string(), -1};
    while (fgets(buf.data(), static_cast<int>(buf.size()), pipe) != nullptr)
        out += buf.data();
    int rc = pclose(pipe);
    int exit_code = WEXITSTATUS(rc);
    return {out, exit_code};
}

static bool value_to_int(const Json::Value &v, int &out)
{
    if (v.isInt() || v.isUInt())
    {
        out = v.asInt();
        return true;
    }
    if (v.isString())
    {
        try {
            out = std::stoi(v.asString());
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

static bool find_point_format_recursive(const Json::Value &node, int &out)
{
    static const std::vector<std::string> keys = {
        "point_format", "pointFormat", "pc_format", "format", "point_format_id"
    };

    if (node.isObject())
    {
        // Najpierw sprawdź bezpośrednio popularne klucze w tym obiekcie
        for (const auto &k : keys)
        {
            if (node.isMember(k) && value_to_int(node[k], out))
                return true;
        }

        // Często info znajduje się w metadata -> readers.las -> [0]
        if (node.isMember("readers.las") && node["readers.las"].isArray() && node["readers.las"].size() > 0)
        {
            const Json::Value &r = node["readers.las"][0];
            for (const auto &k : keys)
            {
                if (r.isMember(k) && value_to_int(r[k], out))
                    return true;
            }
        }

        // Rekurencyjnie przeszukaj pola
        for (const auto &name : node.getMemberNames())
        {
            if (find_point_format_recursive(node[name], out)) return true;
        }
    }
    else if (node.isArray())
    {
        for (const auto &elem : node)
            if (find_point_format_recursive(elem, out)) return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Użycie: " << argv[0] << " \"/pełna/ścieżka/Chmura Zadanie.las\"\n";
        return 1;
    }

    std::string filename = argv[1];
    // uruchom pdal info --metadata (nie zapisujemy niczego do pliku)
    std::string cmd = "pdal info --metadata " + std::string("\"") + filename + std::string("\"");
    auto [out, code] = run_cmd(cmd);

    if (code != 0)
    {
        std::cerr << "Błąd: 'pdal info' zakończył się kodem " << code << "\n";
        std::cerr << "Wyjście pdal:\n" << out << "\n";
        return 2;
    }

    if (out.empty())
    {
        std::cerr << "Brak wyjścia od pdal.\n";
        return 3;
    }

    // Parsuj JSON w pamięci
    Json::CharReaderBuilder rb;
    std::string errs;
    Json::Value root;
    std::istringstream iss(out);
    if (!Json::parseFromStream(rb, iss, &root, &errs))
    {
        std::cerr << "Nie udało się sparsować JSON z pdal info: " << errs << "\n";
        return 4;
    }

    int pf = -1;
    bool found = false;
    if (root.isMember("metadata"))
        found = find_point_format_recursive(root["metadata"], pf);
    if (!found)
        found = find_point_format_recursive(root, pf);

    if (found)
    {
        std::cout << pf << "\n"; // tylko numer Point Format wypisany na konsolę
        return 0;
    }
    else
    {
        std::cout << "Brak informacji o Point Format w metadanych\n";
        return 5;
    }
}