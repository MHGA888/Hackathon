#include <pdal/PointView.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/StageFactory.hpp>

#include <pdal/io/BufferReader.hpp>

#include <vector>

void fillView(pdal::PointViewPtr view)
{
    struct Point
    {
        double x;
        double y;
        double z;
    };

    for (int i = 0; i < 1000; ++i)
    {
        Point p;

        p.x = -93.0 + i*0.001;
        p.y = 42.0 + i*0.001;
        p.z = 106.0 + i;

        view->setField(pdal::Dimension::Id::X, i, p.x);
        view->setField(pdal::Dimension::Id::Y, i, p.y);
        view->setField(pdal::Dimension::Id::Z, i, p.z);
        view->setField(pdal::Dimension::Id::Blue, i , 12);
    }
}

pdal::PointViewSet Transform(pdal::PointViewSet &viewSet){
    pdal::PointViewSet temp;

    for (auto &tempview : viewSet)
    {
        std::cout << "Liczba punktów: " << tempview->size() << "\n";
        pdal::PointId limit = static_cast<pdal::PointId>(tempview->size());
        for (pdal::PointId i = 0; i < limit; ++i)
        {
            // double x = tempview->getFieldAs<double>(pdal::Dimension::Id::X, i);
            // double y = tempview->getFieldAs<double>(pdal::Dimension::Id::Y, i);
            // double z = tempview->getFieldAs<double>(pdal::Dimension::Id::Z, i);
            // double blue = tempview->getFieldAs<double>(pdal::Dimension::Id::Blue, i);
            // double green = tempview->getFieldAs<double>(pdal::Dimension::Id::Green, i);
            // double red = tempview->getFieldAs<double>(pdal::Dimension::Id::Red, i);

            // std::cout << i << ": " << x << ", " << y << ", " << z << "\n";
            //std::cout<<"\r                                                        \rPrzetwarzam punkt nr"<<i;
            tempview->setField(pdal::Dimension::Id::Blue,i,12);
            tempview->setField(pdal::Dimension::Id::Green,i,12);
            tempview->setField(pdal::Dimension::Id::Red,i,12);
        }
    }
    return viewSet;
}

int main(int argc, char* argv[])
{
    using namespace pdal;

    if (argc != 2)
    {
        std::cerr << "Użycie: " << argv[0] << " \"/pełna/ścieżka/Chmura Zadanie.las\"\n";
        return 1;
    }

    std::string filename = argv[1];

    Options options;
    options.add("filename", "output.las");
    Options readerOptions;
    readerOptions.add("filename",filename);

    StageFactory factory;

    Stage *writer = factory.createStage("writers.las");
   

    Stage *reader = factory.createStage("readers.las");
    if (!reader)
    {
        std::cerr << "Nie udało się utworzyć stage 'readers.las'. Upewnij się, że driver jest zainstalowany.\n";
        return 2;
    }
    
    reader->setOptions(readerOptions);

    PointTable table;
    try
    {
        // Jeśli chcesz, żeby writer wykonał pipeline, NIE wywołuj tu reader->execute.
        reader->prepare(table);
    }
    catch (const pdal::pdal_error &e)
    {
        std::cerr << "Błąd podczas prepare(): " << e.what() << "\n";
        return 3;
    }

    pdal::PointViewSet viewSet;
    try
    {
        viewSet = reader->execute(table);
    }
    catch (const pdal::pdal_error &e)
    {
        std::cerr << "Błąd podczas execute(): " << e.what() << "\n";
        return 4;
    }

    // table.layout()->registerDim(Dimension::Id::X);
    // table.layout()->registerDim(Dimension::Id::Y);
    // table.layout()->registerDim(Dimension::Id::Z);
    // table.layout()->registerDim(Dimension::Id::Blue);

    viewSet=Transform(viewSet);

    writer->setOptions(options);

    PointViewPtr view(new PointView(table));
    for(auto &it:viewSet){
        view=it;
        BufferReader buffer;
        buffer.addView(view);

        writer->setInput(buffer);
    
        writer->prepare(table);
        writer->execute(table);
    }
    
    // fillView(view);

    // StageFactory always "owns" stages it creates. They'll be destroyed with the factory.
    
    
    
}