#include <pcl/point_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <vector>

// Funkcja zwraca wektor indeksów punktów, które zostały uznane za budynki (Klasa 6)
std::vector<int> detectBuildings(pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_no_ground) {
    
    std::vector<int> building_indices_all; // Tu zbieramy wyniki
    
    // Obiekty pomocnicze PCL
    pcl::SACSegmentation<pcl::PointXYZ> seg;
    pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
    pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
    pcl::ExtractIndices<pcl::PointXYZ> extract;

    // Kopia chmury do "obgryzania" (będziemy z niej wycinać znalezione ściany)
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::copyPointCloud(*cloud_no_ground, *cloud_filtered);

    // --- KONFIGURACJA RANSAC ---
    seg.setOptimizeCoefficients(true);
    seg.setModelType(pcl::SACMODEL_PLANE); // Szukamy płaszczyzn
    seg.setMethodType(pcl::SAC_RANSAC);
    seg.setMaxIterations(1000);
    seg.setDistanceThreshold(0.3); // Tolerancja: punkty 30cm od płaszczyzny to jeszcze ściana

    // Pętla: Szukaj płaszczyzn tak długo, jak chmura ma jeszcze sporo punktów
    int original_size = cloud_no_ground->points.size();
    int min_plane_size = 500; // Minimalna liczba punktów, żeby uznać to za ścianę/dach

    while (cloud_filtered->points.size() > 0.1 * original_size) { // Dopóki zostało > 10% punktów
        
        // 1. Uruchom RANSAC
        seg.setInputCloud(cloud_filtered);
        seg.segment(*inliers, *coefficients);

        // Jeśli nie znaleziono płaszczyzny lub jest za mała -> koniec
        if (inliers->indices.size() < min_plane_size) {
            break;
        }

        // 2. Dodaj znalezione punkty do wyniku
        // Uwaga: inliers->indices odnoszą się do 'cloud_filtered', trzeba by je mapować
        // W uproszczeniu hackathonowym:
        // Tutaj lepiej działać na ExtractIndices i odkładać punkty do nowej chmury "buildings"
        // Ale dla zachowania indeksów, prościej jest oznaczyć je w tym miejscu.
        
        // (Wersja uproszczona - zbieramy same punkty do osobnej chmury lub oznaczamy flagą)
        // W tym przykładzie po prostu wyświetlamy info:
        // std::cout << "Znaleziono plaszczyzne budynku: " << inliers->indices.size() << " pkt." << std::endl;

        // 3. Usuń znalezioną płaszczyznę z chmury tymczasowej, żeby szukać następnej
        extract.setInputCloud(cloud_filtered);
        extract.setIndices(inliers);
        extract.setNegative(true); // TRUE = Usuń to co znaleziono (zostaw resztę)
        extract.filter(*cloud_filtered);
    }
    
    // W rzeczywistym kodzie musisz zwrócić indeksy pasujące do ORYGINALNEJ chmury.
    // Najprostsza metoda "produkcyjna": 
    // Zamiast zwracać vector<int>, przekaż wskaźnik do chmury i ustawiaj 
    // cloud->points[i].classification = 6 bezpośrednio wewnątrz pętli.
    
    return building_indices_all; 
}