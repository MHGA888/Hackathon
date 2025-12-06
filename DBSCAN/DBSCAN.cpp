#include <pdal/StageFactory.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Stage.hpp>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 4 || argc > 5)
    {
        std::cerr << "Użycie: " << argv[0] << " wejscie.las wyjscie.las eps min_points(optional)\n";
        std::cerr << "  eps: promien sąsiedztwa (np. 0.5)\n";
        std::cerr << "  min_points (opcjonalnie): minimalna liczba punktów w klastrze (domyślnie 6)\n";
        return 1;
    }

    std::string inFile = argv[1];
    std::string outFile = argv[2];
    double eps = std::stod(argv[3]);
    int minPts = 6;
    if (argc == 5) minPts = std::stoi(argv[4]);

    pdal::StageFactory factory;

    // Reader
    pdal::Stage* reader = factory.createStage("readers.las");
    if (!reader) { std::cerr << "Brak readers.las\n"; return 2; }
    pdal::Options ro;
    ro.add("filename", inFile);
    reader->setOptions(ro);

    // DBSCAN filter
    pdal::Stage* dbscan = factory.createStage("filters.dbscan");
    if (!dbscan) { std::cerr << "Brak filters.dbscan\n"; return 3; }
    pdal::Options dbo;
    dbo.add("eps", eps);              // promień
    dbo.add("min_points", static_cast<uint64_t>(minPts)); // minimalna liczba punktów
    // dbo.add("keep_unclassified", true); // opcje zależne od wersji PDAL (jeśli dostępne)
    dbscan->setOptions(dbo);
    dbscan->setInput(*reader);

    // Writer
    pdal::Stage* writer = factory.createStage("writers.las");
    if (!writer) { std::cerr << "Brak writers.las\n"; return 4; }
    pdal::Options wo;
    wo.add("filename", outFile);
    writer->setOptions(wo);
    writer->setInput(*dbscan);

    // Execute pipeline
    pdal::PointTable table;
    try
    {
        writer->prepare(table);
        writer->execute(table);
    }
    catch (const pdal::pdal_error &e)
    {
        std::cerr << "Błąd PDAL: " << e.what() << "\n";
        return 5;
    }

    std::cout << "Zapisano wynik z DBSCAN do: " << outFile << "\n";
    std::cout << "Sprawdź etykiety klastrów: pdal info --metadata " << outFile << "\n";
    return 0;
}