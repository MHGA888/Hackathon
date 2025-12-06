#include <pdal/StageFactory.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Stage.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Użycie: " << argv[0] << " wejscie.pcd wyjscie.las\n";
        return 1;
    }

    std::string inFile = argv[1];
    std::string outFile = argv[2];

    pdal::StageFactory factory;

    // reader
    pdal::Stage* reader = factory.createStage("readers.las");
    if (!reader) { std::cerr << "Brak readers.las\n"; return 2; }
    pdal::Options ro;
    ro.add("filename", inFile);
    reader->setOptions(ro);

    // filters.assign -> ustawia kolory na stałe (stream-safe)
    pdal::Stage* assign = factory.createStage("filters.assign");
    if (!assign) { std::cerr << "Brak filters.assign\n"; return 3; }
    pdal::Options ao;
    // składnia assignments zależy od wersji PDAL; często: "assignment" lub "assignments"
    // próbujemy obu — najczęściej "assignment" jako tablica nie działa z pkg-config; użyj pojedynczego ciągu:
    ao.add("assignment", std::string("Blue=12,Green=12,Red=12"));
    assign->setOptions(ao);
    assign->setInput(reader);

    // writer
    pdal::Stage* writer = factory.createStage("writers.las");
    if (!writer) { std::cerr << "Brak writers.las\n"; return 4; }
    pdal::Options wo;
    wo.add("filename", outFile);
    writer->setOptions(wo);
    writer->setInput(assign);

    // prepare & execute — PDAL wykona pipeline strumieniowo
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

    std::cout << "Gotowe: " << outFile << "\n";
    return 0;
}