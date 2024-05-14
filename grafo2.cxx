#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

struct Punto {
    double x, y;
};

double distanciaEuclidiana(const Punto& a, const Punto& b) {
    return std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

std::vector<std::vector<double>> construirMatrizAdyacencia(const std::vector<Punto>& puntos) {
    int n = puntos.size();
    std::vector<std::vector<double>> matriz(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                matriz[i][j] = distanciaEuclidiana(puntos[i], puntos[j]);
            } else {
                matriz[i][j] = 0.0;
            }
        }
    }
    return matriz;
}

void twoOptSwap(std::vector<Punto>& ruta, int i, int k) {
    while (i < k) {
        std::swap(ruta[i], ruta[k]);
        ++i;
        --k;
    }
}

bool twoOpt(std::vector<Punto>& ruta, const std::vector<std::vector<double>>& matriz) {
    bool improved = false;
    int max_iterations = 10000;
    int iterations = 0;

    do {
        improved = false;
        for (int i = 1; i < ruta.size() - 1; ++i) {
            for (int k = i + 1; k < ruta.size(); ++k) {
                double delta = matriz[i - 1][i] + matriz[k][(k + 1) % ruta.size()] - matriz[i - 1][k] - matriz[i][(k + 1) % ruta.size()];
                if (delta > 0) {
                    twoOptSwap(ruta, i, k);
                    improved = true;
                }
            }
        }
        ++iterations;
    } while (improved && iterations < max_iterations);

    return improved;
}

std::vector<Punto> encontrarRutaMinima(std::vector<Punto>& puntos) {
    if (puntos.size() < 2) {
        return puntos;
    }
    puntos.insert(puntos.begin(), {0, 0}); // Añade el origen al inicio de la lista
    std::vector<Punto> ruta(puntos);
    std::vector<std::vector<double>> matriz = construirMatrizAdyacencia(puntos);
    twoOpt(ruta, matriz);
    ruta.push_back({0, 0}); // Asegura regresar al origen
    return std::vector<Punto>(ruta.begin() + 1, ruta.end() - 1);
}

void guardarGrafos(const std::vector<Punto>& puntos, const std::vector<Punto>& rutaOptimizada, const std::string& archivo) {
    std::ofstream grafoCompleto(archivo + "_grafo_completo.csv");
    std::ofstream caminoOptimo(archivo + "_camino_optimo.csv");

    if (!grafoCompleto.is_open() || !caminoOptimo.is_open()) {
        std::cerr << "Error al abrir los archivos para escribir los grafos." << std::endl;
        return;
    }

    if (puntos.empty() || rutaOptimizada.empty()) {
        std::cerr << "Error: los vectores de puntos o ruta optimizada están vacíos." << std::endl;
        return;
    }

    for (size_t i = 0; i < puntos.size(); ++i) {
        for (size_t j = 0; j < puntos.size(); ++j) {
            if (i != j) { // Asegúrate de no conectar el mismo punto
                grafoCompleto << puntos[i].x << "," << puntos[i].y << "," << puntos[j].x << "," << puntos[j].y << "\n";
            }
        }
    }

    for (size_t i = 0; i < rutaOptimizada.size() - 1; ++i) {
        caminoOptimo << rutaOptimizada[i].x << "," << rutaOptimizada[i].y << "," << rutaOptimizada[i + 1].x << "," << rutaOptimizada[i + 1].y << "\n";
    }

    grafoCompleto.close();
    caminoOptimo.close();
}

void procesarArchivo(const std::string& nombreArchivo) {
    std::ifstream archivoEntrada(nombreArchivo);
    if (!archivoEntrada) {
        std::cerr << "Error al abrir el archivo de entrada: " << nombreArchivo << std::endl;
        return;
    }

    int n;
    archivoEntrada >> n;

    for (int i = 0; i < n; ++i) {
        int m;
        archivoEntrada >> m;
        if (m == 0) continue; // pasa a la siguiente iteración si no hay agujeros
        std::vector<Punto> agujeros(m);
        for (int j = 0; j < m; ++j) {
            archivoEntrada >> agujeros[j].x >> agujeros[j].y;
        }

        std::vector<Punto> rutaOptimizada = encontrarRutaMinima(agujeros);

        // Imprimir la ruta optimizada
        std::cout << "Ruta optimizada para el conjunto " << i + 1 << ":\n";
        for (const auto& p : rutaOptimizada) {
            std::cout << "(" << p.x << ", " << p.y << ")\n";
        }

        guardarGrafos(agujeros, rutaOptimizada, "output_" + std::to_string(i));
    }

    archivoEntrada.close();
}

int main() {
    std::vector<std::string> nombresArchivos = {"in_0.txt"};
    
    for (const auto& nombreArchivo : nombresArchivos) {
        procesarArchivo(nombreArchivo);
    }

    return 0;
}
