#include <pdal/StageFactory.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Stage.hpp>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    // if (argc < 4 || argc > 5)
    // {
    //     std::cerr << "Użycie: " << argv[0] << " wejscie.las wyjscie.las eps min_points(optional)\n";
    //     std::cerr << "  eps: promien sąsiedztwa (np. 0.5)\n";
    //     std::cerr << "  min_points (opcjonalnie): minimalna liczba punktów w klastrze (domyślnie 6)\n";
    //     return 1;
    // }
    if(argc!=3)return -1;
    std::string inFile = argv[1];
    std::string outFile = argv[2];
    // double eps = std::stod(argv[3]);
    // int minPts = 6;
    // if (argc == 5) minPts = std::stoi(argv[4]);

    pdal::StageFactory factory;


    // Reader
    pdal::Stage* reader = factory.createStage("readers.las");
    if (!reader) { std::cerr << "Brak readers.las\n"; return 2; }
    pdal::Options ro;
    ro.add("filename", inFile);
    reader->setOptions(ro);

    
    //noise
    int mean_k = 8;
    float thresh = 2.0;
    pdal::Stage* outlier = factory.createStage("filters.outlier");
    if (!outlier) { std::cerr << "Brak filters.outlier\n"; return 3; }
    pdal::Options dbo;
    dbo.add("mean_k", mean_k);              // promień
    //dbo.add("thresh", thresh); // minimalna liczba punktów
    // dbo.add("keep_unclassified", true); // opcje zależne od wersji PDAL (jeśli dostępne)
    outlier->setOptions(dbo);
    outlier->setInput(*reader);


    //ground
    pdal::Stage* csf = factory.createStage("filters.csf");
    if (!csf) { std::cerr << "Brak filters.csf\n"; return 3; }
    pdal::Options dbo_csf;
    dbo_csf.add("resolution", 1.0);              // promień
    dbo_csf.add("threshold", 0.5); 
    dbo_csf.add("rigidness", 5);
    dbo_csf.add("hdiff", 0.3);
    //dbo_csf.add("returns", "last only");
    //dbo_csf.add("only_ground", 1); //Chyba nie ma znaczenia
    // dbo.add("keep_unclassified", true); // opcje zależne od wersji PDAL (jeśli dostępne)
    csf->setOptions(dbo_csf);
    csf->setInput(*outlier);


    //Sample (real filter)
    pdal::Stage* filter = factory.createStage("filters.sample");
    if (!filter) { std::cerr << "Brak filters.sample\n"; return 3; }
    pdal::Options fo;
    fo.add("where","Classification != 2 && Classification != 7");
    filter->setOptions(fo);
    filter->setInput(*csf);

    //Euclides
    pdal::Stage* cluster = factory.createStage("filters.cluster");
    if(!cluster) { std::cerr << "Brak filters.sample\n"; return 3; }
    pdal::Options co;
    co.add("tolerance", 1.0);
    co.add("min_points",500);
    co.add("max_points",1000000);
    cluster->setOptions(co);
    cluster->setInput(*filter);

    
    //trees
    // pdal::Stage* tree = factory.createStage("filters.litree");
    // if (!tree) { std::cerr << "Brak filters.tree\n"; return 3; }
    // pdal::Options dbo_tree;
    // dbo_tree.add("min_points", 10);              // promień
    // dbo_tree.add("min_height",  3.0); 
    // dbo_tree.add("radius", 100.0);
    // // dbo.add("keep_unclassified", true); // opcje zależne od wersji PDAL (jeśli dostępne)
    // tree->setOptions(dbo_tree);
    // tree->setInput(*cluster);


    // Writer
    pdal::Stage* writer = factory.createStage("writers.las");
    if (!writer) { std::cerr << "Brak writers.las\n"; return 4; }
    pdal::Options wo;
    wo.add("filename", outFile);
    writer->setOptions(wo);
    writer->setInput(*csf);

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