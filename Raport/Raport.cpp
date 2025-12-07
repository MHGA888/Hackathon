#include <pdal/StageFactory.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Stage.hpp>
#include <iostream>
#include <string>
#include <map>

#include <pdal/io/BufferReader.hpp>
#include <pdal/PointView.hpp>

int main(int argc, char* argv[])
{
    
    if(argc!=2)return -1;
    std::string inFile = argv[1];
    
    pdal::StageFactory factory;


    // Reader
    pdal::Stage* reader = factory.createStage("readers.las");
    if (!reader) { std::cerr << "Brak readers.las\n"; return 2; }
    pdal::Options ro;
    ro.add("filename", inFile);
    reader->setOptions(ro);

    pdal::PointTable table;

    reader->prepare(table);

    pdal::PointViewSet view;
    view = reader->execute(table);
    
    int count = 0;
    std::map<int,int> appear;
    for (auto &it : view)
    {
        pdal::PointId limit = static_cast<pdal::PointId>(it->size());
        for (pdal::PointId i = 0; i < limit; ++i)
        {
            count++;
                int cls = it->getFieldAs<int>(pdal::Dimension::Id::Classification, i);
            appear[cls] += 1;
        }
    }
    for(auto [key,val] : appear){
        std::cout<<"Classification "<<key<<" appeared "<<val<<" times. That's "<<(((double)val/count)*100.0)<<"\% of all points"<<std::endl;
    }
    std::cout<<"Number of all points: "<<count<<std::endl;
    return 0;
}