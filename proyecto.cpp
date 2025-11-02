#include <algorithm>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <windows.h>
#include <functional>
#include <conio.h>  
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;



// ============================================================================
// Variables globales
// ============================================================================
string archivoSeleccionado = "";
const string extension = ".csv";

struct Curso {
    string nombre;
    float zona;
    float parcial1;
    float parcial2;
    float examen;
};

struct Estudiante {
    string nombre;
    string codigo;
    vector<Curso> cursos;
};

vector<Estudiante> estudiantes;
string carreraSeccion = "";  
vector<string> cursos;

// ============================================================================
// UTILIDADES DE INTERFAZ
// ============================================================================

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

string pieDePagina() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%H:%M", &tstruct);
    return "║ " + string(buf) + "          Grupo E seccion C ║";
}

string padding(string str, bool extra_space = false, bool center = true) {
    const int CONTENT_WIDTH = 34;
    if (str.length() > CONTENT_WIDTH) {
        str = str.substr(0, CONTENT_WIDTH);
    }

    int total_padding = CONTENT_WIDTH - str.length();
    int left_pad = center ? total_padding / 2 : 0;
    int right_pad = total_padding - left_pad;

    string line = "║" + string(left_pad, ' ') + str + string(right_pad, ' ');
    if (extra_space) line += ' ';
    return line + "║";

}


string padingRight(string str, bool extra_space = false) {
    return padding(str, extra_space, false);
}

string topLinea() { return "╔══════════════════════════════════╗"; }
string bottomLinea() { return "╚══════════════════════════════════╝"; }
string middleLinea() { return "╠══════════════════════════════════╣"; }
string emptyLinea() { return "║                                  ║"; }

string tabla(string str, bool extra_space = false, bool center = true){
    const int CONTENT_WIDTH = 20;
    if (str.length() > CONTENT_WIDTH) {
        str = str.substr(0, CONTENT_WIDTH);
    }

    int total_padding = CONTENT_WIDTH - str.length();
    int left_pad = center ? total_padding / 2 : 0;
    int right_pad = total_padding - left_pad;

    string line = "║" + string(left_pad, ' ') + str + string(right_pad, ' ');
    if (extra_space) line += ' ';
    return line;
}

string divisionLinea(int tamano) { 
    string division = "";
    for(int i = 0; i < tamano; i++){
        division += "═";
    }
    return "║" + division + "║";

}

int calcularTamanoDivision(int limite){
    return (limite*21)-1;
}

void tablaDeTres(vector<string> array){
    string line = "";

    const int limite = 3;
    int tamano = array.size() < limite ? array.size() : limite;
    
    string division = divisionLinea(calcularTamanoDivision(tamano));
    cout << division << endl;

    for(int i = 0; i < array.size(); i+= limite){

        tamano = array.size() - i < limite ? array.size() - i : limite;
        
        for(int j = 0; j < tamano; j++){
            line += tabla(array[i+j]);
            division = divisionLinea(calcularTamanoDivision(tamano));
            
        }    
        line += "║";
        cout << line << endl;
        cout << division << endl;
        line = "";
    }
    
}

// ============================================================================
// FUNCIONES DE GETCH
// ============================================================================

string crearTexto( string texto, function<void(string)> ejecutable) {
    char caracter;

    while ((caracter = _getch()) != '\r' || texto.length() < 0) {
        if(caracter == '\b' && texto.length() > 0){
                texto = texto.substr(0, texto.length() - 1);
        } else if(caracter != '\b' && caracter != '\r') {
            texto += caracter;
        }   

        ejecutable(texto);
    }

    return texto;
}

string crearTextoConValidacion( string texto, function<void(string)> ejecutable, function<bool(string)> validar) {
    char caracter;

    while ((caracter = _getch()) != '\r' || !validar(texto)) {
        if(caracter == '\b' && texto.length() > 0){
                texto = texto.substr(0, texto.length() - 1);
        } else if(caracter != '\b' && caracter != '\r') {
            texto += caracter;
        }   

        ejecutable(texto);
    }

    return texto;
}

int moverOpcion(int opcion, int limite) {
    if (opcion < 0) opcion = limite;
    if (opcion > limite) opcion = 0;
    return opcion;
}

void navegarMenu(int limite, int exitOption, function<void(int)> dibujarMenu, function<void(int)> seleccionarOpcion) {
    char caracter;
    int opcion = 0;
    
    while ((caracter = _getch())) {
        // Detectar teclas especiales (flechas del teclado)
        if (caracter == -32 || caracter == 224 || caracter == 0) {
            char teclaEspecial = _getch();
            if (teclaEspecial == 72) {
                opcion = moverOpcion(opcion - 1, limite);
                dibujarMenu(opcion);
                cout << "Opcion: " << opcion + 1 << endl;
                continue; // Continuar al siguiente ciclo sin procesar más
            }
            if (teclaEspecial == 80) {
                opcion = moverOpcion(opcion + 1, limite);
                dibujarMenu(opcion);
                cout << "Opcion: " << opcion + 1 << endl;
                continue; // Continuar al siguiente ciclo sin procesar más
            }
        }
        
        if (caracter == 'w' || caracter == 'W') opcion = moverOpcion(opcion - 1, limite);
        if (caracter == 's' || caracter == 'S') opcion = moverOpcion(opcion + 1, limite);
        // if (caracter == 'e' || caracter == 'E') break;

        if (caracter == '\r') {
            seleccionarOpcion(opcion);
            if(opcion == exitOption || exitOption == -1) break;
        }
        
        dibujarMenu(opcion);
        cout << "Opcion: " << opcion + 1 << endl;
    }
}


// ============================================================================
// Archivos 
// ============================================================================

bool archivoVacio(string nombreArchivo) {
    ifstream archivo(nombreArchivo, std::ios::binary | std::ios::ate);

    if(!archivo.is_open()){
        cout << "No se pudo abrir el archivo" << endl;
        return true;
    }

    return archivo.tellg() == 0;
}

// Guarda todos los datos en el archivo CSV
void guardarDatos() {
    string nombreArchivo = archivoSeleccionado + extension;
    ofstream archivo(nombreArchivo);
    
    if (!archivo.is_open()) {
        cout << "Error al guardar los datos" << endl;
        return;
    }
    
    // Línea 1: Marcador de compatibilidad
    archivo << "COMPATIBLE" << endl;
    
    // Línea 2: Carrera/Sección
    archivo << carreraSeccion << endl;
    
    // Línea 3: Número de cursos
    archivo << cursos.size() << endl;
    
    // Líneas siguientes: Nombres de los cursos
    for (const auto& curso : cursos) {
        archivo << curso << endl;
    }
    
    // Línea: Número de estudiantes
    archivo << estudiantes.size() << endl;
    
    // Datos de cada estudiante
    for (const auto& estudiante : estudiantes) {
        archivo << estudiante.nombre << endl;
        archivo << estudiante.codigo << endl;
        
        // Calificaciones de cada curso
        for (const auto& curso : estudiante.cursos) {
            archivo << curso.zona << "," 
                   << curso.parcial1 << "," 
                   << curso.parcial2 << "," 
                   << curso.examen << endl;
        }
    }
    
    archivo.close();
}

// Carga todos los datos desde el archivo CSV
void cargarDatos() {
    string nombreArchivo = archivoSeleccionado + extension;
    
    // Verificar si el archivo está vacío o no existe
    if (archivoVacio(nombreArchivo)) {
        return; // No hay datos que cargar
    }
    
    ifstream archivo(nombreArchivo);
    
    if (!archivo.is_open()) {
        cout << "Error al cargar los datos" << endl;
        return;
    }
    
    string linea;
    
    // Línea 1: Marcador de compatibilidad
    getline(archivo, linea);
    if (linea != "COMPATIBLE") {
        cout << "Formato de archivo no compatible" << endl;
        archivo.close();
        return;
    }
    
    // Línea 2: Carrera/Sección
    getline(archivo, carreraSeccion);
    
    // Línea 3: Número de cursos
    int numCursos;
    archivo >> numCursos;
    archivo.ignore(); // Ignorar el salto de línea
    
    // Limpiar el vector de cursos antes de cargar
    cursos.clear();
    
    // Cargar nombres de cursos
    for (int i = 0; i < numCursos; i++) {
        getline(archivo, linea);
        cursos.push_back(linea);
    }
    
    // Número de estudiantes
    int numEstudiantes;
    archivo >> numEstudiantes;
    archivo.ignore(); // Ignorar el salto de línea
    
    // Limpiar el vector de estudiantes antes de cargar
    estudiantes.clear();
    
    // Cargar datos de cada estudiante
    for (int i = 0; i < numEstudiantes; i++) {
        Estudiante est;
        
        getline(archivo, est.nombre);
        getline(archivo, est.codigo);
        
        // Cargar calificaciones de cada curso
        for (int j = 0; j < numCursos; j++) {
            Curso curso;
            curso.nombre = cursos[j];
            
            char coma;
            archivo >> curso.zona >> coma 
                   >> curso.parcial1 >> coma 
                   >> curso.parcial2 >> coma 
                   >> curso.examen;
            archivo.ignore(); // Ignorar el salto de línea
            
            est.cursos.push_back(curso);
        }
        
        estudiantes.push_back(est);
    }
    
    archivo.close();
}

void crearArchivo(string nombreArchivo){
    ofstream archivo;
    archivo.open(nombreArchivo, ios::out);
    
    if(!archivo.is_open()){
        cout << "No se pudo crear el archivo" << endl;
        return;
    }
    archivo << "COMPATIBLE" << endl;
    archivo << carreraSeccion << endl;  // <-- Guardar la carrera/sección

    archivo.close();
}

void UsarCrearArchivo(){
    string nombreArchivo = archivoSeleccionado + extension;

    if(archivoVacio(nombreArchivo)){
        // Preguntar por la carrera/seccións
        cout << "\n¿De que carrera/seccion sera este archivo?: ";
        getline(cin, carreraSeccion);
        crearArchivo(nombreArchivo);
        cout << "Archivo creado para: " << carreraSeccion << endl;
        _getch();
    } else {
        // Cargar los datos existentes
        cargarDatos();
        cout << "Datos cargados correctamente" << endl;
        cout << "Carrera/Seccion: " << carreraSeccion << endl;
        cout << "Cursos: " << cursos.size() << endl;
        cout << "Estudiantes: " << estudiantes.size() << endl;
        _getch();
    }
}




// ============================================================================
// Crear/eliminar cursos
// ============================================================================

// Filtra cursos cuyo nombre contiene el texto dado (búsqueda parcial, insensible a mayúsculas)
vector<string> filtrarCursos(const vector<string>& originales, const string& nombre) {
    vector<string> filtrados;
    string lowerNombre = nombre;
    transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
    for (const auto& curso : originales) {
        string lowerCurso = curso;
        transform(lowerCurso.begin(), lowerCurso.end(), lowerCurso.begin(), ::tolower);
        if (lowerCurso.find(lowerNombre) != string::npos) {
            filtrados.push_back(curso);
        }
    }
    return filtrados;
}

// Muestra la tabla de cursos filtrados y el mensaje de entrada
void mostrarListaCursos(vector<string> cursosLista, string inputNombre) {
    limpiarPantalla();
    
    if (!cursosLista.empty()) {
        tablaDeTres(cursosLista);
    }
    
    string mensaje = "Escriba el nombre del curso";
    bool existe = false;
    string lowerInput = inputNombre;
    transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    
    for(auto nombre : cursosLista){
        string lowerNombre = nombre;
        transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
       
        if (lowerNombre == lowerInput) {
            existe = true;
            break;
        }
    }

    if (existe) {
        mensaje += " (Editar/eliminar)";
    } else {
        mensaje += " (crear)";
    }
    
    cout << mensaje << ": " << inputNombre;
}

// Crea un nuevo curso y lo agrega a todos los estudiantes
void crearCurso(const string& nombre) {
    // Agregar curso al vector global
    cursos.push_back(nombre);
    
    // Agregar el curso a todos los estudiantes con valores inicializados en 0
    for (auto& estudiante : estudiantes) {
        Curso c;
        c.nombre = nombre;
        c.zona = 0.0f;
        c.parcial1 = 0.0f;
        c.parcial2 = 0.0f;
        c.examen = 0.0f;
        estudiante.cursos.push_back(c);
    }
    guardarDatos(); // <-- AGREGAR
}

// Edita el nombre de un curso en la misma posición
void editarCurso(int numCurso) {
    string nuevoNombre;
    cout << "Nuevo nombre del curso: ";
    getline(cin, nuevoNombre);
    
    // Actualizar en el vector global
    cursos[numCurso] = nuevoNombre;
    
    // Actualizar en todos los estudiantes manteniendo la misma posición
    for (auto& estudiante : estudiantes) {
        if (numCurso < estudiante.cursos.size()) {
            estudiante.cursos[numCurso].nombre = nuevoNombre;
            // Los datos (zona, parcial1, parcial2, examen) se mantienen igual
        }
    }
    guardarDatos(); // <-- AGREGAR
}

// Elimina un curso del vector y de todos los estudiantes
void eliminarCurso(int numCurso) {
    // Eliminar del vector global
    cursos.erase(cursos.begin() + numCurso);
    
    // Eliminar de todos los estudiantes
    for (auto& estudiante : estudiantes) {
        if (numCurso < estudiante.cursos.size()) {
            estudiante.cursos.erase(estudiante.cursos.begin() + numCurso);
        }
    }
    guardarDatos(); // <-- AGREGAR
}

// Muestra el menú para seleccionar acción (estilo menuInicio)
void menuSeleccionarAccionCurso(int option = 0) {
    string editar = option == 0 ? " _> Editar" : " 1. Editar";
    string eliminar = option == 1 ? " _> Eliminar" : " 2. Eliminar";

    limpiarPantalla();
    cout << topLinea() << endl;
    cout << padding("Seleccionar accion") << endl;
    cout << middleLinea() << endl;
    cout << padingRight(editar) << endl;
    cout << padingRight(eliminar) << endl;
    cout << emptyLinea() << endl;
    cout << pieDePagina() << endl;
    cout << bottomLinea() << endl;
}

void opcionSeleccionadaSeleccionarAccionCurso(int opcion, int numCurso) {
    switch (opcion) {
        case 1:
            eliminarCurso(numCurso);
            cout << "Curso eliminado de todos los estudiantes." << endl;
            _getch();
            break;
        case 0:
            editarCurso(numCurso);
            cout << "Curso editado en todos los estudiantes." << endl;
            _getch();
            break;
    }
}

// Función principal refactorizada
void seleccionarAccionCurso(int numCurso) {
    int limite = 1;
    auto seleccionarOpcion = [&](int opcion) {
        opcionSeleccionadaSeleccionarAccionCurso(opcion, numCurso);
    };
    limpiarPantalla();
    menuSeleccionarAccionCurso(0);
    navegarMenu(limite, -1, menuSeleccionarAccionCurso, seleccionarOpcion);
}

// Callback para actualizar la UI durante la entrada
void callbackEntradaCurso(string texto) {
    vector<string> filtrados = filtrarCursos(cursos, texto);
    mostrarListaCursos(filtrados, texto);
}

// Función principal refactorizada
void logicaMenuCrearCurso() {
    mostrarListaCursos(cursos, "");
    string nombreCurso = crearTexto("", callbackEntradaCurso);
    
    // Verificar si existe
    bool existe = false;
    int numCurso = 0;
    for (int i = 0; i < cursos.size(); i++) {
        if (cursos[i] == nombreCurso) {
            existe = true;
            numCurso = i;
            break;
        }
    }
    
    if (!existe) {
        // Crear nuevo curso
        crearCurso(nombreCurso);
        cout << "\nCurso creado y agregado a todos los estudiantes." << endl;
        _getch();
    } else {
        // Seleccionar acción (editar o eliminar)
        seleccionarAccionCurso(numCurso);
    }
}


// ============================================================================
// Crear/eliminar estudiantes
// ============================================================================

// Filtra estudiantes cuyo nombre contiene el texto dado (búsqueda parcial, insensible a mayúsculas)
vector<Estudiante> filtrarEstudiantes(const vector<Estudiante>& originales, const string& nombre) {
    vector<Estudiante> filtrados;
    string lowerNombre = nombre;
    transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
    for (const auto& est : originales) {
        string lowerEstNombre = est.nombre;
        transform(lowerEstNombre.begin(), lowerEstNombre.end(), lowerEstNombre.begin(), ::tolower);
        if (lowerEstNombre.find(lowerNombre) != string::npos) {
            filtrados.push_back(est);
        }
    }
    return filtrados;
}

// Muestra la tabla de estudiantes filtrados y el mensaje de entrada
void mostrarListaEstudiantes(vector<Estudiante> ests, string inputNombre) {
    limpiarPantalla();
    vector<string> nombres;
    for (auto est : ests) {
        nombres.push_back(est.nombre);
    }
    
    if (!nombres.empty()) {
        tablaDeTres(nombres);
    }
    
    string mensaje = "Escriba el nombre del estudiante";
    bool existe = false;
    string lowerInput = inputNombre;
    transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    
    for(auto nombre : nombres){
        string lowerNombre = nombre;
        transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
       
        if (lowerNombre == lowerInput) {
            existe = true;
            break;
        }
    }

    if (existe) {
        mensaje += " (Editar/eliminar)";
    } else {
        mensaje += " (crear)";
    }
    
    cout << mensaje << ": " << inputNombre;
}

// Crea un nuevo estudiante con cursos inicializados
void crearEstudiante(const string& nombre, const string& codigo) {
    Estudiante nuevo;
    nuevo.nombre = nombre;
    nuevo.codigo = codigo;
    for (const auto& curso : cursos) {
        Curso c;
        c.nombre = curso;
        c.zona = 0.0f;
        c.parcial1 = 0.0f;
        c.parcial2 = 0.0f;
        c.examen = 0.0f;
        nuevo.cursos.push_back(c);
    }
    estudiantes.push_back(nuevo);
    guardarDatos(); // <-- AGREGAR
}

// Edita el nombre y código de un estudiante
void editarEstudiante(int numEstudiante) {
    cout << "Nuevo nombre: ";
    getline(cin, estudiantes[numEstudiante].nombre);
    cout << "Nuevo codigo: ";
    getline(cin, estudiantes[numEstudiante].codigo);
    guardarDatos(); // <-- AGREGAR
}

// Elimina un estudiante del vector por nombre
void eliminarEstudiante(int numEstudiante) {
    estudiantes.erase(estudiantes.begin() + numEstudiante);
    guardarDatos(); // <-- AGREGAR
}

// Muestra el menú para seleccionar acción (estilo menuInicio)
void menuSeleccionarAccion(int option = 0) {
    string editar = option == 0 ? " _> Editar" : " 1. Editar";
    string eliminar = option == 1 ? " _> Eliminar" : " 2. Eliminar";

    limpiarPantalla();
    cout << topLinea() << endl;
    cout << padding("Seleccionar accion") << endl;
    cout << middleLinea() << endl;
    cout << padingRight(editar) << endl;
    cout << padingRight(eliminar) << endl;
    cout << emptyLinea() << endl;
    cout << pieDePagina() << endl;
    cout << bottomLinea() << endl;
}

void opcionSeleccionadaSeleccionarAccion(int opcion, int numEstudiante) {
    switch (opcion) {
        case 1:
            eliminarEstudiante(numEstudiante);
            cout << "Estudiante eliminado." << endl;
            _getch();
            break;
        case 0:
            editarEstudiante(numEstudiante);
            cout << "Estudiante editado." << endl;
            _getch();
            break;
    }
}

// Función principal refactorizada
void seleccionarAccion(int numEstudiante) {
    int limite = 1;
    auto seleccionarOpcion = [&](int opcion) {
        opcionSeleccionadaSeleccionarAccion(opcion, numEstudiante);
    };
    limpiarPantalla();
    menuSeleccionarAccion(0);
    navegarMenu(limite, -1, menuSeleccionarAccion, seleccionarOpcion);
}

// Callback para actualizar la UI durante la entrada
void callbackEntrada(string texto) {
    vector<Estudiante> filtrados = filtrarEstudiantes(estudiantes, texto);
    mostrarListaEstudiantes(filtrados, texto);
}

// Función principal refactorizada
void logicaMenuCrearEstudiante() {
    
    mostrarListaEstudiantes(estudiantes, "");
    string nombreEstudiante = crearTexto("", callbackEntrada);
    
    // Verificar si existe
    bool existe = false;
    for (auto est : estudiantes) {
        if (est.nombre == nombreEstudiante) {
            existe = true;
            break;
        }
    }
    
    if (!existe) {
        // Crear nuevo
        string codigo;
        cout << "\nCodigo del estudiante: ";
        getline(cin, codigo);
        crearEstudiante(nombreEstudiante, codigo);
        cout << "Estudiante creado." << endl;
        _getch();
    } else {
        // Encontrar y seleccionar acción
        int numEstudiante = 0;
        for (auto& est : estudiantes) {
            if (est.nombre == nombreEstudiante) {
                seleccionarAccion(numEstudiante);
                break;
            }
            numEstudiante++;
        }
    }
}


// ============================================================================
// Calificaciones - con funciones de alineación
// ============================================================================

// Función auxiliar para repetir caracteres Unicode
string repetirCaracter(const string& caracter, int veces) {
    string resultado = "";
    for (int i = 0; i < veces; i++) {
        resultado += caracter;
    }
    return resultado;
}

// Función para alinear números en celdas de tabla (similar a padding)
string alinearNumero(float numero, int ancho = 10, bool centrado = true) {
    string numStr = to_string((int)numero);
    
    if (numStr.length() > ancho) {
        numStr = numStr.substr(0, ancho);
    }
    
    int total_padding = ancho - numStr.length();
    int left_pad = centrado ? total_padding / 2 : total_padding;
    int right_pad = total_padding - left_pad;
    
    return string(left_pad, ' ') + numStr + string(right_pad, ' ');
}

// Función para alinear texto en celdas de tabla
string alinearTexto(string texto, int ancho, bool centrado = false) {
    if (texto.length() > ancho) {
        texto = texto.substr(0, ancho);
    }
    
    int total_padding = ancho - texto.length();
    int left_pad = centrado ? total_padding / 2 : 0;
    int right_pad = total_padding - left_pad;
    
    return string(left_pad, ' ') + texto + string(right_pad, ' ');
}

// Calcula la nota total de un curso (zona + parcial1 + parcial2 + examen)
float calcularNotaTotal(Curso curso) {
    return curso.zona + curso.parcial1 + curso.parcial2 + curso.examen;
}

// Función reutilizable para validar entrada de calificaciones dentro de un rango
float validarCalificacion(string mensaje, float min, float max) {
    float valorIngresado;
    do {
        cout << mensaje;
        cin >> valorIngresado;
        if (cin.fail() || valorIngresado < min || valorIngresado > max) {
            cout << "Error: El valor debe estar entre " << (int)min << " y " << (int)max << ". Intente nuevamente." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        } else {
            cin.ignore(); // Limpiar buffer después de entrada válida
            return valorIngresado;
        }
    } while (true);
}

// Encuentra el índice de un curso por nombre (case-insensitive)
int encontrarIndiceCurso(const string& nombreCurso) {
    string lowerNombre = nombreCurso;
    transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
    
    for (int i = 0; i < cursos.size(); i++) {
        string lowerCurso = cursos[i];
        transform(lowerCurso.begin(), lowerCurso.end(), lowerCurso.begin(), ::tolower);
        if (lowerCurso == lowerNombre) {
            return i;
        }
    }
    return -1;
}

// Encuentra el índice de un estudiante por nombre (case-insensitive)
int encontrarIndiceEstudiante(const string& nombreEstudiante) {
    string lowerNombre = nombreEstudiante;
    transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
    
    for (int i = 0; i < estudiantes.size(); i++) {
        string lowerEst = estudiantes[i].nombre;
        transform(lowerEst.begin(), lowerEst.end(), lowerEst.begin(), ::tolower);
        if (lowerEst == lowerNombre) {
            return i;
        }
    }
    return -1;
}

// Función reutilizable para mostrar la tabla con todos los estudiantes y sus notas
void mostrarTablaCalificaciones(const string& inputEstudiante, int indiceCurso) {
    limpiarPantalla();
    
    int numCursos = cursos.size();
    const int ANCHO_NOMBRE = 20;
    const int ANCHO_NOTA = 12;
    const int ANCHO_TOTAL = 10;
    
    // Encabezado superior
    cout << "╔";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╦";
    for (int i = 0; i < numCursos; i++) {
        cout << repetirCaracter("═", ANCHO_NOTA) << "╦";
    }
    cout << repetirCaracter("═", ANCHO_TOTAL) << "╗" << endl;
    
    // Encabezado con nombres de cursos
    cout << "║" << alinearTexto("Nombres", ANCHO_NOMBRE, false) << "║";
    for (const auto& curso : cursos) {
        cout << alinearTexto(curso, ANCHO_NOTA, true) << "║";
    }
    cout << alinearTexto("Promedio", ANCHO_TOTAL, true) << "║" << endl;
    
    // Línea separadora
    cout << "╠";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╬";
    for (int i = 0; i < numCursos; i++) {
        cout << repetirCaracter("═", ANCHO_NOTA) << "╬";
    }
    cout << repetirCaracter("═", ANCHO_TOTAL) << "╣" << endl;
    
    // Fila para cada estudiante
    for (const auto& est : estudiantes) {
        // Nombre del estudiante
        string nombreMostrar = est.nombre;
        
        // Verificar si es el estudiante que se está escribiendo
        string lowerInput = inputEstudiante;
        string lowerNombre = est.nombre;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
        
        if (lowerNombre == lowerInput && !inputEstudiante.empty()) {
            nombreMostrar += "(" + est.codigo + ")";
        }
        
        cout << "║" << alinearTexto(nombreMostrar, ANCHO_NOMBRE, false) << "║";
        
        // Mostrar notas de cada curso
        float totalGeneral = 0;
        for (int i = 0; i < est.cursos.size(); i++) {
            float notaTotal = calcularNotaTotal(est.cursos[i]);
            totalGeneral += notaTotal;
            
            // Si es el curso seleccionado, destacar
            if (i == indiceCurso && indiceCurso != -1) {
                string notaStr = ">" + to_string((int)notaTotal) + "<";
                cout << alinearTexto(notaStr, ANCHO_NOTA, true) << "║";
            } else {
                cout << alinearNumero(notaTotal, ANCHO_NOTA, true) << "║";
            }
        }
        
        // Total general
        cout << alinearNumero(totalGeneral / est.cursos.size(), ANCHO_TOTAL, true) << "║" << endl;
    }
    
    // Línea inferior
    cout << "╚";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╩";
    for (int i = 0; i < numCursos; i++) {
        cout << repetirCaracter("═", ANCHO_NOTA) << "╩";
    }
    cout << repetirCaracter("═", ANCHO_TOTAL) << "╝" << endl;
}

// Muestra lista de cursos mientras el usuario escribe
void mostrarSeleccionCurso(const string& inputCurso) {
    limpiarPantalla();
    
    vector<string> cursosFiltrados = filtrarCursos(cursos, inputCurso);
    
    if (!cursosFiltrados.empty()) {
        tablaDeTres(cursosFiltrados);
    }
    
    int indiceCurso = encontrarIndiceCurso(inputCurso);
    string mensaje = "Escriba el nombre de la materia";
    
    if (indiceCurso != -1) {
        mensaje += " [ENCONTRADA]";
    } else if (!inputCurso.empty()) {
        mensaje += " [NO EXISTE - No puede continuar]";
    }
    
    cout << mensaje << ": " << inputCurso;
}

// Callback para actualizar la lista de cursos mientras se escribe
void callbackCursoCalificaciones(string texto) {
    mostrarSeleccionCurso(texto);
}

// Variables globales para callbacks
string nombreCursoGlobal = "";
int indiceCursoGlobal = -1;

void callbackEstudianteCalificaciones(string texto) {
    mostrarTablaCalificaciones(texto, indiceCursoGlobal);
    
    int indiceEst = encontrarIndiceEstudiante(texto);
    string mensaje = "\nEscriba el nombre del estudiante";
    
    if (indiceEst != -1) {
        mensaje += " [ENCONTRADO - Presione Enter]";
    } else if (!texto.empty()) {
        mensaje += " [NO EXISTE - No puede continuar]";
    }
    
    cout << mensaje << ": " << texto;
}

// Menú para editar las 4 calificaciones (zona, parcial1, parcial2, examen)
void menuEditarCalificaciones(int indiceEstudiante, int indiceCurso) {
    Curso& curso = estudiantes[indiceEstudiante].cursos[indiceCurso];
    
    limpiarPantalla();
    cout << topLinea() << endl;
    cout << padding("Editar Calificaciones") << endl;
    cout << middleLinea() << endl;
    cout << padingRight("Estudiante: " + estudiantes[indiceEstudiante].nombre) << endl;
    cout << padingRight("Codigo: " + estudiantes[indiceEstudiante].codigo) << endl;
    cout << padingRight("Materia: " + curso.nombre) << endl;
    cout << middleLinea() << endl;
    
    cout << padingRight("Zona actual: " + to_string((int)curso.zona)) << endl;
    cout << padingRight("Parcial 1 actual: " + to_string((int)curso.parcial1)) << endl;
    cout << padingRight("Parcial 2 actual: " + to_string((int)curso.parcial2)) << endl;
    cout << padingRight("Examen actual: " + to_string((int)curso.examen)) << endl;
    cout << middleLinea() << endl;
    
    float notaTotal = calcularNotaTotal(curso);
    cout << padingRight("TOTAL: " + to_string((int)notaTotal)) << endl;
    cout << emptyLinea() << endl;
    cout << pieDePagina() << endl;
    cout << bottomLinea() << endl;
    
    // Pedir nuevas calificaciones con validación
    curso.zona = validarCalificacion("\nNueva Zona (0-30): ", 0, 30);
    curso.parcial1 = validarCalificacion("Nuevo Parcial 1 (0-15): ", 0, 15);
    curso.parcial2 = validarCalificacion("Nuevo Parcial 2 (0-15): ", 0, 15);
    curso.examen = validarCalificacion("Nuevo Examen (0-40): ", 0, 40);
        
    // Guardar cambios
    guardarDatos();
    
    notaTotal = calcularNotaTotal(curso);
    cout << "\n¡Calificaciones actualizadas!" << endl;
    cout << "Nota total: " << (int)notaTotal << endl;
    _getch();
}

// Función principal de calificaciones
void logicaMenuCalificaciones() {
    // Paso 1: Seleccionar materia
    mostrarSeleccionCurso("");
    string nombreCurso = crearTexto("", callbackCursoCalificaciones);
    
    int indiceCurso = encontrarIndiceCurso(nombreCurso);
    
    // Validar que la materia exista
    if (indiceCurso == -1) {
        cout << "\nLa materia no existe. Presione cualquier tecla..." << endl;
        _getch();
        return;
    }
    
    // Guardar en variables globales para el callback
    nombreCursoGlobal = nombreCurso;
    indiceCursoGlobal = indiceCurso;
    
    // Paso 2: Seleccionar estudiante con la tabla visible
    mostrarTablaCalificaciones("", indiceCurso);
    cout << "\nEscriba el nombre del estudiante: ";
    string nombreEstudiante = crearTexto("", callbackEstudianteCalificaciones);
    
    int indiceEstudiante = encontrarIndiceEstudiante(nombreEstudiante);
    
    // Validar que el estudiante exista
    if (indiceEstudiante == -1) {
        cout << "\nEl estudiante no existe. Presione cualquier tecla..." << endl;
        _getch();
        return;
    }
    
    // Paso 3: Editar calificaciones
    menuEditarCalificaciones(indiceEstudiante, indiceCurso);
}

// ============================================================================
// PROMEDIOS
// ============================================================================

// Función para determinar si un estudiante está aprobado (mínimo 61)
bool estaAprobado(float promedio) {
    return promedio >= 61.0f;
}

// Función para obtener el estado de aprobación como string
string obtenerEstadoAprobacion(float promedio) {
    return estaAprobado(promedio) ? "APROBADO" : "REPROBADO";
}

// Función para obtener el estado de aprobación como string simplificado
string obtenerEstadoAprobacionSimplificado(float promedio) {
    return estaAprobado(promedio) ? "A" : "R";
}

// Función para mostrar tabla de promedios de un estudiante específico
void mostrarPromediosEstudiante(const string& inputEstudiante) {
    limpiarPantalla();
    
    int numCursos = cursos.size();
    const int ANCHO_NOMBRE = 20;
    const int ANCHO_NOTA = 8;
    const int ANCHO_ESTADO_MATERIA = 3;
    const int ANCHO_TOTAL = 10;
    const int ANCHO_ESTADO_GENERAL = 12;
    
    // Encabezado superior
    cout << "╔";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╦";
    for (int i = 0; i < numCursos; i++) {
        cout << repetirCaracter("═", ANCHO_NOTA) << "╦";
        cout << repetirCaracter("═", ANCHO_ESTADO_MATERIA) << "╦";
    }
    cout << repetirCaracter("═", ANCHO_TOTAL) << "╦" << repetirCaracter("═", ANCHO_ESTADO_GENERAL) << "╗" << endl;
    
    // Encabezado con nombres de cursos
    cout << "║" << alinearTexto("Nombres", ANCHO_NOMBRE, false) << "║";
    for (const auto& curso : cursos) {
        cout << alinearTexto(curso, ANCHO_NOTA, true) << "║";
        cout << alinearTexto("E", ANCHO_ESTADO_MATERIA, true) << "║";
    }
    cout << alinearTexto("Promedio", ANCHO_TOTAL, true) << "║" << alinearTexto("Estado Gen", ANCHO_ESTADO_GENERAL, true) << "║" << endl;
    
    // Línea separadora
    cout << "╠";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╬";
    for (int i = 0; i < numCursos; i++) {
        cout << repetirCaracter("═", ANCHO_NOTA) << "╬";
        cout << repetirCaracter("═", ANCHO_ESTADO_MATERIA) << "╬";
    }
    cout << repetirCaracter("═", ANCHO_TOTAL) << "╬" << repetirCaracter("═", ANCHO_ESTADO_GENERAL) << "╣" << endl;
    
    // Mostrar solo estudiantes que coincidan con la búsqueda
    bool encontrado = false;
    for (const auto& est : estudiantes) {
        string lowerInput = inputEstudiante;
        string lowerNombre = est.nombre;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        transform(lowerNombre.begin(), lowerNombre.end(), lowerNombre.begin(), ::tolower);
        
        if (lowerInput.empty() || lowerNombre.find(lowerInput) != string::npos) {
            encontrado = true;
            
            // Nombre del estudiante
            string nombreMostrar = est.nombre + "(" + est.codigo + ")";
            cout << "║" << alinearTexto(nombreMostrar, ANCHO_NOMBRE, false) << "║";
            
            // Mostrar notas y estados de cada curso
            float totalGeneral = 0;
            for (int i = 0; i < est.cursos.size(); i++) {
                float notaTotal = calcularNotaTotal(est.cursos[i]);
                totalGeneral += notaTotal;
                string estadoMateria = obtenerEstadoAprobacionSimplificado(notaTotal);
                
                cout << alinearNumero(notaTotal, ANCHO_NOTA, true) << "║";
                cout << alinearTexto(estadoMateria, ANCHO_ESTADO_MATERIA, true) << "║";
            }
            
            // Promedio general
            float promedioGeneral = totalGeneral / est.cursos.size();
            cout << alinearNumero(promedioGeneral, ANCHO_TOTAL, true) << "║";
            
            // Estado general de aprobación
            string estadoGeneral = obtenerEstadoAprobacion(promedioGeneral);
            cout << alinearTexto(estadoGeneral, ANCHO_ESTADO_GENERAL, true) << "║" << endl;
        }
    }
    
    if (!encontrado && !inputEstudiante.empty()) {
        int totalAncho = ANCHO_NOMBRE + numCursos * (ANCHO_NOTA + ANCHO_ESTADO_MATERIA) + ANCHO_TOTAL + ANCHO_ESTADO_GENERAL;
        cout << "║" << alinearTexto("No se encontraron estudiantes", totalAncho, true) << "║" << endl;
    }
    
    // Línea inferior
    cout << "╚";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╩";
    for (int i = 0; i < numCursos; i++) {
        cout << repetirCaracter("═", ANCHO_NOTA) << "╩";
        cout << repetirCaracter("═", ANCHO_ESTADO_MATERIA) << "╩";
    }
    cout << repetirCaracter("═", ANCHO_TOTAL) << "╩" << repetirCaracter("═", ANCHO_ESTADO_GENERAL) << "╝" << endl;
}

// Callback para actualizar la tabla de promedios mientras se escribe
void callbackPromediosEstudiante(string texto) {
    mostrarPromediosEstudiante(texto);
    
    int indiceEst = encontrarIndiceEstudiante(texto);
    string mensaje = "\nEscriba el nombre del estudiante";
    
    if (indiceEst != -1) {
        mensaje += " [ENCONTRADO - Presione Enter para generar CSV]";
    } else if (!texto.empty()) {
        mensaje += " [NO EXISTE - No puede continuar]";
    }
    
    cout << mensaje << ": " << texto;
}

// Función para generar CSV de un estudiante específico
void generarCSVEstudiante(const string& nombreEstudiante) {
    int indiceEstudiante = encontrarIndiceEstudiante(nombreEstudiante);
    
    if (indiceEstudiante == -1) {
        cout << "\nEstudiante no encontrado." << endl;
        _getch();
        return;
    }
    
    const Estudiante& est = estudiantes[indiceEstudiante];
    string nombreArchivo = "promedios_" + est.nombre + "_" + est.codigo + ".csv";
    
    // Limpiar nombre de archivo de caracteres no válidos
    for (char& c : nombreArchivo) {
        if (c == ' ' || c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    
    ofstream archivo(nombreArchivo);
    
    if (!archivo.is_open()) {
        cout << "\nError al crear el archivo CSV." << endl;
        _getch();
        return;
    }
    
    // Encabezado del CSV
    archivo << "Estudiante,Codigo,Carrera/Seccion,";
    for (const auto& curso : cursos) {
        archivo << curso << ",";
    }
    archivo << "Promedio General,Estado" << endl;
    
    // Datos del estudiante
    archivo << est.nombre << "," << est.codigo << "," << carreraSeccion << ",";
    
    float totalGeneral = 0;
    for (const auto& curso : est.cursos) {
        float notaTotal = calcularNotaTotal(curso);
        totalGeneral += notaTotal;
        archivo << notaTotal << ",";
    }
    
    float promedioGeneral = totalGeneral / est.cursos.size();
    archivo << promedioGeneral << "," << obtenerEstadoAprobacion(promedioGeneral) << endl;
    
    archivo.close();
    
    cout << "\nCSV generado exitosamente: " << nombreArchivo << endl;
    cout << "Promedio general: " << (int)promedioGeneral << " - " << obtenerEstadoAprobacion(promedioGeneral) << endl;
    _getch();
}

// Función para mostrar promedios por materia
void mostrarPromediosMateria(const string& inputMateria) {
    limpiarPantalla();
    
    // Si no hay texto escrito, mostrar todas las materias disponibles
    if (inputMateria.empty()) {
        if (!cursos.empty()) {
            tablaDeTres(cursos);
        }
        cout << "Escriba el nombre de la materia: ";
        return;
    }
    
    int indiceMateria = encontrarIndiceCurso(inputMateria);
    
    if (indiceMateria == -1) {
        cout << "Materia no encontrada: " << inputMateria << endl;
        return;
    }
    
    const int ANCHO_NOMBRE = 25;
    const int ANCHO_NOTA = 12;
    const int ANCHO_ESTADO = 12;
    const int ANCHO_PROMEDIO = 12;
    
    // Encabezado superior
    cout << "╔";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╦";
    cout << repetirCaracter("═", ANCHO_NOTA) << "╦";
    cout << repetirCaracter("═", ANCHO_ESTADO) << "╦";
    cout << repetirCaracter("═", ANCHO_PROMEDIO) << "╗" << endl;
    
    // Encabezado
    cout << "║" << alinearTexto("Estudiante", ANCHO_NOMBRE, false) << "║";
    cout << alinearTexto("Nota", ANCHO_NOTA, true) << "║";
    cout << alinearTexto("Estado", ANCHO_ESTADO, true) << "║";
    cout << alinearTexto("Promedio Clase", ANCHO_PROMEDIO, true) << "║" << endl;
    
    // Línea separadora
    cout << "╠";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╬";
    cout << repetirCaracter("═", ANCHO_NOTA) << "╬";
    cout << repetirCaracter("═", ANCHO_ESTADO) << "╬";
    cout << repetirCaracter("═", ANCHO_PROMEDIO) << "╣" << endl;
    
    float totalClase = 0;
    int estudiantesConNota = 0;
    
    // Calcular promedio de la clase primero
    for (const auto& est : estudiantes) {
        if (indiceMateria < est.cursos.size()) {
            float notaTotal = calcularNotaTotal(est.cursos[indiceMateria]);
            totalClase += notaTotal;
            estudiantesConNota++;
        }
    }
    
    float promedioClase = estudiantesConNota > 0 ? totalClase / estudiantesConNota : 0;
    
    // Mostrar cada estudiante
    for (const auto& est : estudiantes) {
        if (indiceMateria < est.cursos.size()) {
            float notaTotal = calcularNotaTotal(est.cursos[indiceMateria]);
            string estado = obtenerEstadoAprobacion(notaTotal);
            
            string nombreMostrar = est.nombre + "(" + est.codigo + ")";
            cout << "║" << alinearTexto(nombreMostrar, ANCHO_NOMBRE, false) << "║";
            cout << alinearNumero(notaTotal, ANCHO_NOTA, true) << "║";
            cout << alinearTexto(estado, ANCHO_ESTADO, true) << "║";
            cout << alinearNumero(promedioClase, ANCHO_PROMEDIO, true) << "║" << endl;
        }
    }
    
    // Línea inferior
    cout << "╚";
    cout << repetirCaracter("═", ANCHO_NOMBRE) << "╩";
    cout << repetirCaracter("═", ANCHO_NOTA) << "╩";
    cout << repetirCaracter("═", ANCHO_ESTADO) << "╩";
    cout << repetirCaracter("═", ANCHO_PROMEDIO) << "╝" << endl;
    
    cout << "\nPromedio de la clase: " << (int)promedioClase << endl;
    cout << "Estudiantes evaluados: " << estudiantesConNota << endl;
}

// Callback para actualizar la tabla de promedios por materia
void callbackPromediosMateria(string texto) {
    limpiarPantalla();
    
    // Buscar materias que coincidan con el texto escrito
    vector<string> materiasFiltradas = filtrarCursos(cursos, texto);
    
    // Mostrar materias filtradas
    if (!materiasFiltradas.empty()) {
        tablaDeTres(materiasFiltradas);
    }
    
    int indiceCurso = encontrarIndiceCurso(texto);
    string mensaje = "\nEscriba el nombre de la materia";
    
    if (indiceCurso != -1) {
        mensaje += " [ENCONTRADA - Presione Enter para generar CSV]";
    } else if (!texto.empty() && materiasFiltradas.empty()) {
        mensaje += " [NO EXISTE - No puede continuar]";
    } else if (!texto.empty()) {
        mensaje += " [Presione Enter para seleccionar]";
    }
    
    cout << mensaje << ": " << texto;
}

// Función para generar CSV de una materia específica
void generarCSVMateria(const string& nombreMateria) {
    int indiceMateria = encontrarIndiceCurso(nombreMateria);
    
    if (indiceMateria == -1) {
        cout << "\nMateria no encontrada." << endl;
        _getch();
        return;
    }
    
    string nombreArchivo = "promedios_" + nombreMateria + ".csv";
    
    // Limpiar nombre de archivo de caracteres no válidos
    for (char& c : nombreArchivo) {
        if (c == ' ' || c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    
    ofstream archivo(nombreArchivo);
    
    if (!archivo.is_open()) {
        cout << "\nError al crear el archivo CSV." << endl;
        _getch();
        return;
    }
    
    // Encabezado del CSV
    archivo << "Materia,Carrera/Seccion,Estudiante,Codigo,Nota,Estado" << endl;
    
    float totalClase = 0;
    int estudiantesConNota = 0;
    
    // Datos de cada estudiante en la materia
    for (const auto& est : estudiantes) {
        if (indiceMateria < est.cursos.size()) {
            float notaTotal = calcularNotaTotal(est.cursos[indiceMateria]);
            string estado = obtenerEstadoAprobacion(notaTotal);
            
            archivo << nombreMateria << "," << carreraSeccion << "," 
                   << est.nombre << "," << est.codigo << "," 
                   << notaTotal << "," << estado << endl;
            
            totalClase += notaTotal;
            estudiantesConNota++;
        }
    }
    
    // Agregar promedio de la clase
    float promedioClase = estudiantesConNota > 0 ? totalClase / estudiantesConNota : 0;
    archivo << nombreMateria << "," << carreraSeccion << "," 
           << "PROMEDIO_CLASE" << "," << "N/A" << "," 
           << promedioClase << "," << obtenerEstadoAprobacion(promedioClase) << endl;
    
    archivo.close();
    
    cout << "\nCSV generado exitosamente: " << nombreArchivo << endl;
    cout << "Promedio de la clase: " << (int)promedioClase << " - " << obtenerEstadoAprobacion(promedioClase) << endl;
    cout << "Estudiantes evaluados: " << estudiantesConNota << endl;
    _getch();
}

// Menú de promedios (estilo menuInicio)
void menuPromedios(int option = 0) {
    string estudiante_txt = option == 0 ? " _> Promedios por Estudiante" : " 1. Promedios por Estudiante";
    string materia_txt = option == 1 ? " _> Promedios por Materia" : " 2. Promedios por Materia";
    string salir = option == 2 ? " _> Salir" : " 3. Salir";

    limpiarPantalla();
    cout << topLinea() << endl;
    cout << padding("Modulo de Promedios") << endl;
    cout << middleLinea() << endl;
    cout << padingRight(estudiante_txt) << endl;
    cout << padingRight(materia_txt) << endl;
    cout << padingRight(salir) << endl;
    cout << emptyLinea() << endl;
    cout << pieDePagina() << endl;
    cout << bottomLinea() << endl;
}

// Función principal del módulo de promedios
void logicaMenuPromedios() {
    int limite = 2;
    auto seleccionarOpcion = [&](int opcion) {
        switch (opcion) {
            case 0: {
                // Promedios por estudiante
                mostrarPromediosEstudiante("");
                cout << "\nEscriba el nombre del estudiante: ";
                string nombreEstudiante = crearTexto("", callbackPromediosEstudiante);
                
                int indiceEstudiante = encontrarIndiceEstudiante(nombreEstudiante);
                
                if (indiceEstudiante != -1) {
                    generarCSVEstudiante(nombreEstudiante);
                } else {
                    cout << "\nEstudiante no encontrado. Presione cualquier tecla..." << endl;
                    _getch();
                }
                break;
            }
            case 1: {
                // Promedios por materia
                mostrarPromediosMateria("");
                cout << "\nEscriba el nombre de la materia: ";
                
                // Validar que la materia exista antes de permitir Enter
                auto validarMateria = [](const string& texto) -> bool {
                    return encontrarIndiceCurso(texto) != -1;
                };
                
                string nombreMateria = crearTextoConValidacion("", callbackPromediosMateria, validarMateria);
                
                // Mostrar la tabla con estudiantes y notas de la materia seleccionada
                mostrarPromediosMateria(nombreMateria);
                cout << "\nPresione Enter para generar CSV...";
                _getch();
                
                generarCSVMateria(nombreMateria);
                break;
            }
        }
    };
    
    limpiarPantalla();
    menuPromedios(0);
    navegarMenu(limite, 2, menuPromedios, seleccionarOpcion);
}

// ============================================================================
// MENU INICIO
// ============================================================================

void menuAccesoBloqueado(){
    limpiarPantalla();
    cout << topLinea() << endl;
    cout << padding("ACCESO BLOQUEADO") << endl;
    cout << middleLinea() << endl;
    cout << padding("Debe crear al menos:") << endl;
    cout << padding("- 1 Estudiante") << endl;
    cout << padding("- 1 Curso") << endl;
    cout << emptyLinea() << endl;
    cout << padding("Presione cualquier tecla...") << endl;
    cout << bottomLinea() << endl;
    _getch();   
}

void menuInicio(int option = 0) {
    bool puedeAcceder = !estudiantes.empty() && !cursos.empty();
    
    string estudiantes_txt = option == 0 ? " _> Crear/eliminar estudiantes" : " 1. Crear/eliminar estudiantes";
    string cursos_txt = option == 1 ? " _> Crear/eliminar cursos" : " 2. Crear/eliminar cursos";
    
    // Deshabilitar opciones 3 y 4 si no hay estudiantes o cursos
    string calificaciones, promedios;
    if (!puedeAcceder) {
        calificaciones = option == 2 ? " _> Calificaciones [BLOQUEADO]" : " 3. Calificaciones [BLOQUEADO]";
        promedios = option == 3 ? " _> Promedios [BLOQUEADO]" : " 4. Promedios [BLOQUEADO]";
    } else {
        calificaciones = option == 2 ? " _> Calificaciones" : " 3. Calificaciones";
        promedios = option == 3 ? " _> Promedios" : " 4. Promedios";
    }
    
    string salir = option == 4 ? " _> Salir" : " 5. Salir";

    limpiarPantalla();
    cout << topLinea() << endl;
    // cout << padding("Universidad Mariano Galvez") << endl;
    
    // Mostrar carrera/sección si existe
    if (!carreraSeccion.empty()) {
        // cout << middleLinea() << endl;
        cout << padding(carreraSeccion) << endl;
    }
    
    cout << middleLinea() << endl;
    cout << padingRight(estudiantes_txt) << endl;
    cout << padingRight(cursos_txt) << endl;
    cout << padingRight(calificaciones) << endl;
    cout << padingRight(promedios) << endl;
    cout << padingRight(salir) << endl;
    cout << emptyLinea() << endl;
    
    // Mostrar advertencia si no pueden acceder a calificaciones/promedios
    if (!puedeAcceder) {
        cout << padding("Cree al menos 1 estudiante") << endl;
        cout << padding("y 1 curso para continuar") << endl;
    }
    
    cout << pieDePagina() << endl;
    cout << bottomLinea() << endl;
}


void opcionSeleccionadaMenuInicio(int opcion){
    bool puedeAcceder = !estudiantes.empty() && !cursos.empty();
    
    switch (opcion) {
        case 0:
            logicaMenuCrearEstudiante();
            break;
        case 1:
            logicaMenuCrearCurso();
            break;
        case 2:
            if (puedeAcceder) {
                logicaMenuCalificaciones();
            } else {
                menuAccesoBloqueado();
            }
            break;
        case 3:
            if (puedeAcceder) {
                logicaMenuPromedios();
            } else {
                menuAccesoBloqueado();
            }
            break;
    }
}

void logicaMenuInicio() {
    char caracter;
    int limite = 4;

    menuInicio(0);
    cout << "Opcion: " << 0 << endl;

    navegarMenu(limite, 4, menuInicio, opcionSeleccionadaMenuInicio);
}

// ============================================================================
// Escojer archivo
// ============================================================================
vector<string> buscarArchivos() {
    vector<string> archivos;
    string directorio = ".";
    
    try {
        for (auto entrada : fs::directory_iterator(directorio)) {
            if (!entrada.is_regular_file()) continue;
            if (entrada.path().extension() != extension) continue;
            
            string nombreArchivo = entrada.path().filename().string();

            nombreArchivo = nombreArchivo.substr(0, nombreArchivo.length() - extension.length());
            
            archivos.push_back(nombreArchivo);
            
        }
    } catch (const fs::filesystem_error& e) {
        cout << "Error al acceder al directorio: " << e.what() << endl;
    }
    
    return archivos;
}

vector<string> buscarArchivo(string nombreArchivo) {
    vector<string> archivos = buscarArchivos();
    vector<string> archivosFiltrados;
    
    for (int i = 0; i < archivos.size(); i++) {
        if (archivos[i].find(nombreArchivo) != string::npos) {
            archivosFiltrados.push_back(archivos[i]);
        }
    }
    
    return archivosFiltrados;
}

void ejecutable(string texto) {
    vector<string> archivos = buscarArchivos();
    vector<string> archivosFiltrados;
        
    archivosFiltrados = buscarArchivo(texto);

    limpiarPantalla();
    
    tablaDeTres(archivosFiltrados);
    cout << "Escoja un archivo o crear uno nuevo (<Enter>): " << texto;   
}

void menuEscojerArchivo() {
    limpiarPantalla();
   
    vector<string> archivos = buscarArchivos();

    string texto = "";

    tablaDeTres(archivos);
    cout << "Escoja un archivo o crear uno nuevo (<Enter>): " << texto;

    archivoSeleccionado = crearTexto(texto, ejecutable);
    
    cout << endl;
    UsarCrearArchivo();

}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    SetConsoleOutputCP(CP_UTF8); 
    SetConsoleCP(CP_UTF8); 

    menuEscojerArchivo();

    logicaMenuInicio();
    


    return 0;
}

