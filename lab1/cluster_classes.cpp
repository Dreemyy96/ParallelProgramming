#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// Класс GpuSpec
class GpuSpec {
public:
    string Name;
    int Memory; // в MB
    int Cores;

    // Конструктор по умолчанию
    GpuSpec() : Name("Unknown"), Memory(0), Cores(0) {}

    GpuSpec(const string& name, int memory, int cores)
        : Name(name), Memory(memory), Cores(cores) {}


    // Метод для вывода данных
    void Print() const {
        cout << "GPU: " << Name << ", Memory: " << Memory << "MB, Cores: " << Cores << endl;
    }

    // Метод для экспорта данных в файл
    void Export(ofstream& out) const {
        out << Name << " " << Memory << " " << Cores << endl;
    }

    // Метод для импорта данных из файла
    void Import(ifstream& in) {
        in >> Name >> Memory >> Cores;
    }
};

// Класс CpuSpec
class CpuSpec {
public:
    string Name;
    int Cores;
    double Frequency; // в GHz

    CpuSpec() : Name("Unknown"), Cores(0), Frequency(0.0) {}

    CpuSpec(const string& name, int cores, double frequency)
        : Name(name), Cores(cores), Frequency(frequency) {}

    void Print() const {
        cout << "CPU: " << Name << ", Cores: " << Cores << ", Frequency: " << Frequency << "GHz" << endl;
    }

    void Export(ofstream& out) const {
        out << Name << " " << Cores << " " << Frequency << endl;
    }

    void Import(ifstream& in) {
        in >> Name >> Cores >> Frequency;
    }
};

// Класс RamSpec
class RamSpec {
public:
    string Type;
    int Size; // в MB

    RamSpec() : Type("Unknown"), Size(0) {}

    RamSpec(const string& type, int size)
        : Type(type), Size(size) {}


    void Print() const {
        cout << "RAM: " << Type << ", Size: " << Size << "MB" << endl;
    }

    void Export(ofstream& out) const {
        out << Type << " " << Size << endl;
    }

    void Import(ifstream& in) {
        in >> Type >> Size;
    }
};

// Класс LanSpec
class LanSpec {
public:
    string AdapterName;
    double Speed; // в Gbps

    LanSpec() : AdapterName("Unknown"), Speed(0.0) {}

    LanSpec(const string& adapterName, double speed)
        : AdapterName(adapterName), Speed(speed) {}

    void Print() const {
        cout << "LAN: " << AdapterName << ", Speed: " << Speed << "Gbps" << endl;
    }

    void Export(ofstream& out) const {
        out << AdapterName << " " << Speed << endl;
    }

    void Import(ifstream& in) {
        in >> AdapterName >> Speed;
    }
};

// Класс ClusterNode
class ClusterNode {
public:
    CpuSpec Cpu;
    GpuSpec Gpu;
    RamSpec Ram;
    LanSpec Lan;

    void Print() const {
        cout << "Cluster Node:" << endl;
        Cpu.Print();
        Gpu.Print();
        Ram.Print();
        Lan.Print();
    }

    void Export(ofstream& out) const {
        Cpu.Export(out);
        Gpu.Export(out);
        Ram.Export(out);
        Lan.Export(out);
    }

    void Import(ifstream& in) {
        Cpu.Import(in);
        Gpu.Import(in);
        Ram.Import(in);
        Lan.Import(in);
    }
};

// Класс Cluster
class Cluster {
public:
    vector<ClusterNode> Nodes;

    void AddNode(const ClusterNode& node) {
        Nodes.push_back(node);
    }

    void Print() const {
        cout << "Cluster contains " << Nodes.size() << " nodes:" << endl;
        for (const auto& node : Nodes) {
            node.Print();
            cout << endl;
        }
    }

    void Export(const string& filename) const {
        ofstream out(filename);
        if (out.is_open()) {
            out << Nodes.size() << endl;
            for (const auto& node : Nodes) {
                node.Export(out);
            }
            out.close();
        } else {
            cerr << "Error opening file for export!" << endl;
        }
    }

    void Import(const string& filename) {
        ifstream in(filename);
        if (in.is_open()) {
            size_t nodeCount;
            if (!(in >> nodeCount)) {
                cerr << "Error reading node count" << endl;
                return;
            }

            Nodes.resize(nodeCount);
            for (auto& node : Nodes) {
                node.Import(in);
                if (in.fail()) {
                    cerr << "Error reading node data" << endl;
                    break;
                }
            }
            in.close();
        } else {
            cerr << "Error opening file for import!" << endl;
        }
    }
};




int main() {
    Cluster cluster;

    // Создаем узел кластера
    ClusterNode node1;
    node1.Cpu = {"Intel-Xeon", 8, 3.6};
    node1.Gpu = {"NvidiaRtx3080", 10240, 8704};
    node1.Ram = {"DDR4", 32768};
    node1.Lan = {"IntelEthernet", 1.0};

    // Добавляем узел в кластер
    cluster.AddNode(node1);

    // Печать данных
    cluster.Print();

    // Экспорт данных в файл
    cluster.Export("cluster_data.txt");

    // Импорт данных из файла
    Cluster importedCluster;
    importedCluster.Import("cluster_data.txt");

    cout<<"Print out file!\n";
    importedCluster.Print();

    return 0;
}
