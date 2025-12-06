#include <pdal/StageFactory.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Stage.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Użycie: " << argv[0] << " wejscie.las wyjscie.pcd\n";
        return 1;
    }

    std::string inFile = argv[1];
    std::string outFile = argv[2];

    pdal::StageFactory factory;

    pdal::Stage* reader = factory.createStage("readers.las");
    if (!reader) { std::cerr << "Brak readers.las\n"; return 2; }
    pdal::Options ro;
    ro.add("filename", inFile);
    reader->setOptions(ro);

    pdal::Stage* writer = factory.createStage("writers.pcd");
    if (!writer) { std::cerr << "Brak writers.pcd\n"; return 4; }
    pdal::Options wo;
    wo.add("filename", outFile);
    writer->setOptions(wo);
    writer->setInput(*reader);

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