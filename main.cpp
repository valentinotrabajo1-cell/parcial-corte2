#include <iostream>
#include <string>
#include <ctime>

#define FILAS 22
#define COLUMNAS 22

#define PARED 0
#define VIA 1
#define LIBRE 2
#define OCUPADO 3
#define MOTO 4
#define DISCAP 5
#define ENTRADA 6
#define SALIDA 7

#define MAX 50
#define TARIFA_MINUTO 50

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define WHITE "\033[37m"

struct Vehiculo {
    std::string placa;
    time_t entrada;
    int fila;
    int col;
    bool activo;
};

int mapa[FILAS][COLUMNAS];
Vehiculo veh[MAX];
int ocupados = 0;
int total = 0;

// =====================
// FUNCION CON PUNTEROS
// =====================
bool buscarEspacioLibre(int *f, int *c) {

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {

            if (mapa[i][j] == LIBRE || mapa[i][j] == DISCAP || mapa[i][j] == MOTO) {
                *f = i;
                *c = j;
                return true;
            }
        }
    }

    return false;
}

void inicializarMapa() {

    for (int i = 0; i < FILAS; i++)
        for (int j = 0; j < COLUMNAS; j++)
            mapa[i][j] = PARED;

    mapa[0][1] = ENTRADA;
    mapa[0][19] = SALIDA;

    for (int j = 1; j < COLUMNAS - 1; j++) {
        mapa[1][j] = VIA;
        mapa[20][j] = VIA;
    }

    for (int i = 1; i < FILAS - 1; i++) {
        mapa[i][1] = VIA;
        mapa[i][10] = VIA;
        mapa[i][20] = VIA;
    }

    for (int i = 2; i < 20; i++) {

        for (int j = 2; j < 9; j++) {
            if (i % 3 == 0) mapa[i][j] = VIA;
            else {
                mapa[i][j] = LIBRE;
                total++;
            }
        }

        for (int j = 9; j < 15; j++) {
            if (i % 3 == 0) mapa[i][j] = VIA;
            else {
                mapa[i][j] = DISCAP;
                total++;
            }
        }

        for (int j = 15; j < 20; j++) {
            if (i % 3 == 0) mapa[i][j] = VIA;
            else {
                mapa[i][j] = MOTO;
                total++;
            }
        }
    }
}

void mostrarMapa() {

    std::cout << "\n===== MAPA PARQUEADERO =====\n\n";

    for (int j = 0; j < COLUMNAS; j++)
        std::cout << j % 10 << " ";
    std::cout << "\n";

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {

            if (mapa[i][j] == PARED) std::cout << WHITE << "# " << RESET;
            else if (mapa[i][j] == VIA) std::cout << WHITE << ". " << RESET;
            else if (mapa[i][j] == LIBRE) std::cout << GREEN << "L " << RESET;
            else if (mapa[i][j] == OCUPADO) std::cout << RED << "O " << RESET;
            else if (mapa[i][j] == MOTO) std::cout << BLUE << "M " << RESET;
            else if (mapa[i][j] == DISCAP) std::cout << YELLOW << "D " << RESET;
            else if (mapa[i][j] == ENTRADA) std::cout << "E ";
            else if (mapa[i][j] == SALIDA) std::cout << "S ";
        }
        std::cout << "\n";
    }

    time_t ahora = time(0);
    std::cout << "\nHora: " << ctime(&ahora);
    std::cout << "Ocupados: " << ocupados << "/" << total << "\n";
}

int buscar(std::string p) {
    for (int i = 0; i < MAX; i++)
        if (veh[i].activo && veh[i].placa == p)
            return i;
    return -1;
}

int libre() {
    for (int i = 0; i < MAX; i++)
        if (!veh[i].activo)
            return i;
    return -1;
}

void entrada() {

    std::string p;
    int f, c;

    std::cout << "Placa: ";
    std::cin >> p;

    int opcion;
    std::cout << "1. Elegir espacio manual\n";
    std::cout << "2. Asignar automaticamente\n";
    std::cout << "Opcion: ";
    std::cin >> opcion;

    if (opcion == 2) {
        if (!buscarEspacioLibre(&f, &c)) {
            std::cout << "No hay espacios disponibles\n";
            return;
        }
        std::cout << "Asignado: " << f << "," << c << "\n";
    } else {
        std::cout << "Fila: ";
        std::cin >> f;
        std::cout << "Columna: ";
        std::cin >> c;
    }

    if (f < 0 || f >= FILAS || c < 0 || c >= COLUMNAS) {
        std::cout << "Posicion invalida\n";
        return;
    }

    if (mapa[f][c] == VIA ||
        mapa[f][c] == PARED ||
        mapa[f][c] == ENTRADA ||
        mapa[f][c] == SALIDA) {

        std::cout << "No se puede parquear en zona de movilizacion\n";
        return;
    }

    if (mapa[f][c] == OCUPADO) {
        std::cout << "Espacio ocupado\n";
        return;
    }

    int i = libre();
    if (i == -1) return;

    veh[i].placa = p;
    veh[i].entrada = time(0);
    veh[i].fila = f;
    veh[i].col = c;
    veh[i].activo = true;

    mapa[f][c] = OCUPADO;
    ocupados++;

    std::cout << "\n--- TIQUETE ---\n";
    std::cout << "Placa: " << p << "\n";
    std::cout << "Hora: " << ctime(&veh[i].entrada);
}

void salida() {

    std::string p;
    std::cout << "Placa: ";
    std::cin >> p;

    int i = buscar(p);
    if (i == -1) return;

    int min = (time(0) - veh[i].entrada) / 60;
    if (min < 1) min = 1;

    std::cout << "\n--- SALIDA ---\n";
    std::cout << "Placa: " << p << "\n";
    std::cout << "Tiempo: " << min << " min\n";
    std::cout << "Total: $" << min * TARIFA_MINUTO << "\n";

    mapa[veh[i].fila][veh[i].col] = LIBRE;
    veh[i].activo = false;
    ocupados--;
}

void stats() {
    std::cout << "\n--- ESTADISTICAS ---\n";
    std::cout << "Ocupados: " << ocupados << "\n";
    std::cout << "Disponibles: " << total - ocupados << "\n";
}

int main() {

    system("chcp 65001");
    inicializarMapa();

    int op = 0;

    while (op != 5) {

        std::cout << "\n========================\n";
        std::cout << "   MENU PARQUEADERO\n";
        std::cout << "========================\n";
        std::cout << "1. Ver mapa\n";
        std::cout << "2. Entrada vehiculo\n";
        std::cout << "3. Salida vehiculo\n";
        std::cout << "4. Estadisticas\n";
        std::cout << "5. Salir\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) mostrarMapa();
        else if (op == 2) entrada();
        else if (op == 3) salida();
        else if (op == 4) stats();
    }
}
