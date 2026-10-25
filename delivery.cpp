#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <cstdlib>
using namespace std;

const string ARCHIVO = "delivery.txt";
const double TARIFA_BASE = 3.0;
const double TARIFA_KM = 1.5;

const int MAX_TIENDAS = 20;
const int MAX_CLIENTES = 50;
const int MAX_MENSAJEROS = 20;
const int MAX_ORDENES = 100;
const int MAX_ITEMS = 20;
const int MAX_LINEAS = 20;

bool esNumero(string s) {
	if (s.length() == 0) return false;
	for (int i = 0; i < (int)s.length(); i++) {
		char c = s[i];
		if (!((c >= '0' && c <= '9') || c == '.' || c == '-')) return false;
	}
	return true;
}

string leerTexto(string mensaje) {
	cout << mensaje;
	string s;
	getline(cin, s);
	return s;
}

int leerEntero(string mensaje) {
	string s = leerTexto(mensaje);
	while (!esNumero(s)) {
		cout << "  Valor invalido, intente de nuevo." << endl;
		s = leerTexto(mensaje);
	}
	return atoi(s.c_str());
}

double leerDecimal(string mensaje) {
	string s = leerTexto(mensaje);
	while (!esNumero(s)) {
		cout << "  Valor invalido, intente de nuevo." << endl;
		s = leerTexto(mensaje);
	}
	return atof(s.c_str());
}

string leerLinea(ifstream &f) {
	string s;
	getline(f, s);
	return s;
}

void limpiarPantalla() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void pausa() {
	leerTexto("\n  Presione ENTER para continuar...");
}

void titulo(string texto) {
	cout << "------------------------------------------" << endl;
	cout << "   " << texto << endl;
	cout << "------------------------------------------" << endl;
}

class Ubicacion {
private:
	double latitud;
	double longitud;
public:
	Ubicacion() {
		latitud = 0;
		longitud = 0;
	}
	Ubicacion(double lat, double lon) {
		latitud = lat;
		longitud = lon;
	}
	
	double getLatitud() { return latitud; }
	double getLongitud() { return longitud; }
	
	double distanciaKm(Ubicacion otra) {
		double R = 6371.0;
		double rad = 3.14159265358979 / 180.0;
		double dLat = (otra.latitud - latitud) * rad;
		double dLon = (otra.longitud - longitud) * rad;
		double a = sin(dLat / 2) * sin(dLat / 2) +
			cos(latitud * rad) * cos(otra.latitud * rad) *
			sin(dLon / 2) * sin(dLon / 2);
		return R * 2 * atan2(sqrt(a), sqrt(1 - a));
	}
	
	void mostrar() {
		cout << setprecision(4);
		cout << "(" << latitud << ", " << longitud << ")";
		cout << setprecision(2);
	}
};

class Entidad {
protected:
	int id;
	string nombre;
	Ubicacion ubicacion;
public:
	Entidad() {
		id = 0;
		nombre = "";
	}
	Entidad(int i, string n, Ubicacion u) {
		id = i;
		nombre = n;
		ubicacion = u;
	}
	virtual ~Entidad() {}
	
	virtual void mostrar() = 0;
	
	int getId() { return id; }
	string getNombre() { return nombre; }
	Ubicacion getUbicacion() { return ubicacion; }
	void setUbicacion(Ubicacion u) { ubicacion = u; }
};

void mostrarEntidad(Entidad* e) {
	e->mostrar();
}

struct ItemMenu {
	string nombre;
	double precio;
};

class Tienda : public Entidad {
private:
	ItemMenu menu[MAX_ITEMS];
	int numItems;
public:
	Tienda() : Entidad() {
		numItems = 0;
	}
	Tienda(int i, string n, Ubicacion u) : Entidad(i, n, u) {
		numItems = 0;
	}
	
	void agregarItem(string nombreItem, double precio) {
		if (numItems >= MAX_ITEMS) {
			cout << "  El menu esta lleno." << endl;
			return;
		}
		menu[numItems].nombre = nombreItem;
		menu[numItems].precio = precio;
		numItems++;
	}
	
	int getNumItems() { return numItems; }
	ItemMenu getItem(int pos) { return menu[pos]; }
	
	void mostrarMenu() {
		cout << "  Menu de " << nombre << ":" << endl;
		if (numItems == 0) cout << "   (sin items)" << endl;
		for (int i = 0; i < numItems; i++) {
			cout << "   " << i + 1 << ". " << menu[i].nombre
				<< " - S/ " << menu[i].precio << endl;
		}
	}
	
	void mostrar() {
		cout << "  [" << id << "] " << nombre << " - GPS ";
		ubicacion.mostrar();
		cout << " - " << numItems << " items" << endl;
	}
};

class Cliente : public Entidad {
public:
	Cliente() : Entidad() {}
	Cliente(int i, string n, Ubicacion u) : Entidad(i, n, u) {}
	
	void mostrar() {
		cout << "  [" << id << "] " << nombre << " - GPS ";
		ubicacion.mostrar();
		cout << endl;
	}
};

class Mensajero : public Entidad {
private:
	string vehiculo;
	bool disponible;
public:
	Mensajero() : Entidad() {
		vehiculo = "";
		disponible = true;
	}
	Mensajero(int i, string n, string v, Ubicacion u, bool d) : Entidad(i, n, u) {
		vehiculo = v;
		disponible = d;
	}
	
	string getVehiculo() { return vehiculo; }
	bool estaDisponible() { return disponible; }
	void setDisponible(bool d) { disponible = d; }
	
	void mostrar() {
		cout << "  [" << id << "] " << nombre << " (" << vehiculo << ") - GPS ";
		ubicacion.mostrar();
		if (disponible) cout << " - DISPONIBLE" << endl;
		else cout << " - OCUPADO" << endl;
	}
};

enum Estado { PENDIENTE, PREPARANDO, EN_CAMINO, ENTREGADO };

string nombreEstado(int e) {
	if (e == PENDIENTE) return "Pendiente";
	if (e == PREPARANDO) return "Preparando";
	if (e == EN_CAMINO) return "En camino";
	return "Entregado";
}

struct LineaOrden {
	string nombre;
	double precio;
	int cantidad;
};

class Orden {
private:
	int id;
	int idTienda;
	int idCliente;
	int idMensajero;
	int estado;
	double distanciaKm;
	double tarifa;
	LineaOrden lineas[MAX_LINEAS];
	int numLineas;
public:
	Orden() {
		id = 0;
		idTienda = 0;
		idCliente = 0;
		idMensajero = 0;
		estado = PENDIENTE;
		distanciaKm = 0;
		tarifa = 0;
		numLineas = 0;
	}
	
	Orden(int i, int t, int c, double dist) {
		id = i;
		idTienda = t;
		idCliente = c;
		idMensajero = 0;
		estado = PENDIENTE;
		distanciaKm = dist;
		tarifa = TARIFA_BASE + TARIFA_KM * dist;
		numLineas = 0;
	}
	
	Orden(int i, int t, int c, int m, int e, double dist, double tar) {
		id = i;
		idTienda = t;
		idCliente = c;
		idMensajero = m;
		estado = e;
		distanciaKm = dist;
		tarifa = tar;
		numLineas = 0;
	}
	
	void agregarLinea(string nombre, double precio, int cantidad) {
		if (numLineas >= MAX_LINEAS) {
			cout << "  El carrito esta lleno." << endl;
			return;
		}
		lineas[numLineas].nombre = nombre;
		lineas[numLineas].precio = precio;
		lineas[numLineas].cantidad = cantidad;
		numLineas++;
	}
	
	int getId() { return id; }
	int getIdTienda() { return idTienda; }
	int getIdCliente() { return idCliente; }
	int getIdMensajero() { return idMensajero; }
	void setMensajero(int m) { idMensajero = m; }
	int getEstado() { return estado; }
	double getDistanciaKm() { return distanciaKm; }
	double getTarifa() { return tarifa; }
	int getNumLineas() { return numLineas; }
	LineaOrden getLinea(int pos) { return lineas[pos]; }
	
	double subtotal() {
		double s = 0;
		for (int i = 0; i < numLineas; i++) {
			s = s + lineas[i].precio * lineas[i].cantidad;
		}
		return s;
	}
	
	double total() {
		return subtotal() + tarifa;
	}
	
	bool avanzarEstado() {
		if (estado == ENTREGADO) return false;
		estado = estado + 1;
		return true;
	}
	
	void mostrarTrazado() {
		cout << "  Trazado orden #" << id << ": ";
		for (int e = PENDIENTE; e <= ENTREGADO; e++) {
			if (e <= estado) cout << "[X] ";
			else cout << "[ ] ";
			cout << nombreEstado(e);
			if (e < ENTREGADO) cout << " -> ";
		}
		cout << endl;
	}
};

class SistemaDelivery {
private:
	Tienda tiendas[MAX_TIENDAS];
	Cliente clientes[MAX_CLIENTES];
	Mensajero mensajeros[MAX_MENSAJEROS];
	Orden ordenes[MAX_ORDENES];
	int numTiendas;
	int numClientes;
	int numMensajeros;
	int numOrdenes;
	
	Ubicacion leerUbicacion() {
		double lat = leerDecimal("  Latitud (ej. -15.8402): ");
		double lon = leerDecimal("  Longitud (ej. -70.0219): ");
		return Ubicacion(lat, lon);
	}
	
public:
	SistemaDelivery() {
		numTiendas = 0;
		numClientes = 0;
		numMensajeros = 0;
		numOrdenes = 0;
	}
	
	void registrarTienda() {
		titulo("REGISTRAR TIENDA");
		if (numTiendas >= MAX_TIENDAS) {
			cout << "  No caben mas tiendas." << endl;
			return;
		}
		string nombre = leerTexto("  Nombre de la tienda/restaurante: ");
		Ubicacion u = leerUbicacion();
		Tienda t(numTiendas + 1, nombre, u);
		cout << "\n  Agregue items al menu (deje el nombre vacio para terminar)" << endl;
		while (true) {
			string item = leerTexto("   Item: ");
			if (item == "") break;
			double precio = leerDecimal("   Precio S/: ");
			t.agregarItem(item, precio);
		}
		tiendas[numTiendas] = t;
		numTiendas++;
		guardar();
		cout << "\n  Tienda registrada." << endl;
	}
	
	int registrarCliente() {
		titulo("REGISTRO DE CLIENTE");
		if (numClientes >= MAX_CLIENTES) {
			cout << "  No caben mas clientes." << endl;
			return 0;
		}
		string nombre = leerTexto("  Su nombre: ");
		Ubicacion u = leerUbicacion();
		clientes[numClientes] = Cliente(numClientes + 1, nombre, u);
		numClientes++;
		guardar();
		cout << "\n  Registro exitoso. Su ID de cliente es: " << numClientes << endl;
		return numClientes;
	}
	
	bool existeCliente(int id) {
		return id >= 1 && id <= numClientes;
	}
	
	string nombreCliente(int id) {
		return clientes[id - 1].getNombre();
	}
	
	void registrarMensajero() {
		titulo("REGISTRAR MENSAJERO");
		if (numMensajeros >= MAX_MENSAJEROS) {
			cout << "  No caben mas mensajeros." << endl;
			return;
		}
		string nombre = leerTexto("  Nombre del chofer: ");
		string vehiculo = leerTexto("  Vehiculo (ej. Moto Honda placa ABC-123): ");
		Ubicacion u = leerUbicacion();
		mensajeros[numMensajeros] = Mensajero(numMensajeros + 1, nombre, vehiculo, u, true);
		numMensajeros++;
		guardar();
		cout << "\n  Mensajero registrado." << endl;
	}
	
	void listarTiendas() {
		cout << "\n  TIENDAS" << endl;
		if (numTiendas == 0) cout << "  (no hay tiendas)" << endl;
		for (int i = 0; i < numTiendas; i++) mostrarEntidad(&tiendas[i]);
	}
	
	void listarClientes() {
		cout << "\n  CLIENTES" << endl;
		if (numClientes == 0) cout << "  (no hay clientes)" << endl;
		for (int i = 0; i < numClientes; i++) mostrarEntidad(&clientes[i]);
	}
	
	void listarMensajeros() {
		cout << "\n  MENSAJEROS" << endl;
		if (numMensajeros == 0) cout << "  (no hay mensajeros)" << endl;
		for (int i = 0; i < numMensajeros; i++) mostrarEntidad(&mensajeros[i]);
	}
	
	void verTiendas() {
		titulo("TIENDAS REGISTRADAS");
		listarTiendas();
	}
	void verClientes() {
		titulo("CLIENTES REGISTRADOS");
		listarClientes();
	}
	void verMensajeros() {
		titulo("MENSAJEROS REGISTRADOS");
		listarMensajeros();
	}
	
	void verMenu() {
		titulo("VER MENU DE UNA TIENDA");
		if (numTiendas == 0) {
			cout << "  No hay tiendas registradas." << endl;
			return;
		}
		listarTiendas();
		int idT = leerEntero("\n  ID de la tienda: ");
		if (idT < 1 || idT > numTiendas) {
			cout << "  ID invalido." << endl;
			return;
		}
		cout << endl;
		tiendas[idT - 1].mostrarMenu();
	}
	
	void crearOrden(int idC) {
		titulo("HACER UN PEDIDO");
		if (numTiendas == 0) {
			cout << "  Todavia no hay tiendas registradas." << endl;
			return;
		}
		if (numOrdenes >= MAX_ORDENES) {
			cout << "  No caben mas ordenes." << endl;
			return;
		}
		listarTiendas();
		int idT = leerEntero("\n  ID de la tienda: ");
		if (idT < 1 || idT > numTiendas) {
			cout << "  ID invalido." << endl;
			return;
		}
		
		double dist = tiendas[idT - 1].getUbicacion().distanciaKm(clientes[idC - 1].getUbicacion());
		Orden orden(numOrdenes + 1, idT, idC, dist);
		
		cout << endl;
		tiendas[idT - 1].mostrarMenu();
		cout << "\n  Elija items (0 para terminar)" << endl;
		while (true) {
			int num = leerEntero("   Numero de item: ");
			if (num == 0) break;
			if (num < 1 || num > tiendas[idT - 1].getNumItems()) {
				cout << "   Item inexistente." << endl;
				continue;
			}
			int cant = leerEntero("   Cantidad: ");
			if (cant <= 0) {
				cout << "   Cantidad invalida." << endl;
				continue;
			}
			ItemMenu it = tiendas[idT - 1].getItem(num - 1);
			orden.agregarLinea(it.nombre, it.precio, cant);
		}
		if (orden.getNumLineas() == 0) {
			cout << "  Pedido vacio, cancelado." << endl;
			return;
		}
		
		ordenes[numOrdenes] = orden;
		numOrdenes++;
		guardar();
		cout << "\n  Pedido #" << orden.getId() << " creado (Pendiente)" << endl;
		trazarDistancia(orden);
		mostrarResumen(orden);
	}
	
	void trazarDistancia(Orden o) {
		Tienda t = tiendas[o.getIdTienda() - 1];
		Cliente c = clientes[o.getIdCliente() - 1];
		cout << "\n  Ruta: " << t.getNombre() << " ";
		t.getUbicacion().mostrar();
		cout << "\n        ----(" << o.getDistanciaKm() << " km)-> " << c.getNombre() << " ";
		c.getUbicacion().mostrar();
		cout << endl;
	}
	
	void mostrarResumen(Orden o) {
		cout << "\n  Detalle:" << endl;
		for (int i = 0; i < o.getNumLineas(); i++) {
			LineaOrden l = o.getLinea(i);
			cout << "   - " << l.cantidad << " x " << l.nombre
				<< " (S/ " << l.precio << ")" << endl;
		}
		cout << "  Subtotal: S/ " << o.subtotal() << endl;
		cout << "  Tarifa de entrega: S/ " << o.getTarifa() << endl;
		cout << "  TOTAL: S/ " << o.total() << endl;
	}
	
	void asignarMensajero() {
		titulo("ASIGNAR MENSAJERO");
		if (numOrdenes == 0) {
			cout << "  No hay ordenes." << endl;
			return;
		}
		listarOrdenes();
		int idO = leerEntero("\n  ID de la orden: ");
		if (idO < 1 || idO > numOrdenes) {
			cout << "  Orden inexistente." << endl;
			return;
		}
		if (ordenes[idO - 1].getIdMensajero() != 0) {
			cout << "  Esa orden ya tiene mensajero." << endl;
			return;
		}
		
		Ubicacion origen = tiendas[ordenes[idO - 1].getIdTienda() - 1].getUbicacion();
		int mejor = -1;
		double menorDist = 0;
		for (int i = 0; i < numMensajeros; i++) {
			if (mensajeros[i].estaDisponible() == false) continue;
			double d = origen.distanciaKm(mensajeros[i].getUbicacion());
			if (mejor == -1 || d < menorDist) {
				mejor = i;
				menorDist = d;
			}
		}
		if (mejor == -1) {
			cout << "  No hay mensajeros disponibles." << endl;
			return;
		}
		
		ordenes[idO - 1].setMensajero(mensajeros[mejor].getId());
		mensajeros[mejor].setDisponible(false);
		guardar();
		cout << "\n  Mensajero asignado: " << mensajeros[mejor].getNombre()
			<< " (a " << menorDist << " km de la tienda)" << endl;
	}
	
	void avanzarOrden() {
		titulo("AVANZAR ESTADO DE LA ORDEN");
		if (numOrdenes == 0) {
			cout << "  No hay ordenes." << endl;
			return;
		}
		listarOrdenes();
		int idO = leerEntero("\n  ID de la orden: ");
		if (idO < 1 || idO > numOrdenes) {
			cout << "  Orden inexistente." << endl;
			return;
		}
		
		if (ordenes[idO - 1].getEstado() == ENTREGADO) {
			cout << "  La orden ya fue entregada." << endl;
			return;
		}
		if (ordenes[idO - 1].getEstado() == PREPARANDO && ordenes[idO - 1].getIdMensajero() == 0) {
			cout << "  Asigne un mensajero antes de enviarla." << endl;
			return;
		}
		
		ordenes[idO - 1].avanzarEstado();
		cout << "\n  Nuevo estado: " << nombreEstado(ordenes[idO - 1].getEstado()) << endl;
		
		if (ordenes[idO - 1].getEstado() == ENTREGADO) {
			int posM = ordenes[idO - 1].getIdMensajero() - 1;
			int posC = ordenes[idO - 1].getIdCliente() - 1;
			mensajeros[posM].setDisponible(true);
			mensajeros[posM].setUbicacion(clientes[posC].getUbicacion());
			cout << "  " << mensajeros[posM].getNombre() << " vuelve a estar disponible." << endl;
		}
		ordenes[idO - 1].mostrarTrazado();
		guardar();
	}
	
	void mostrarLineaOrden(int pos) {
		cout << "  #" << ordenes[pos].getId()
			<< " | " << tiendas[ordenes[pos].getIdTienda() - 1].getNombre()
			<< " -> " << clientes[ordenes[pos].getIdCliente() - 1].getNombre()
			<< " | " << nombreEstado(ordenes[pos].getEstado())
			<< " | Mensajero: ";
		if (ordenes[pos].getIdMensajero() == 0) cout << "sin asignar";
		else cout << mensajeros[ordenes[pos].getIdMensajero() - 1].getNombre();
		cout << " | S/ " << ordenes[pos].total() << endl;
	}
	
	void listarOrdenes() {
		cout << "\n  ORDENES" << endl;
		if (numOrdenes == 0) cout << "  (no hay ordenes)" << endl;
		for (int i = 0; i < numOrdenes; i++) mostrarLineaOrden(i);
	}
	
	void verTrazadoOrden() {
		titulo("TRAZADO DE UNA ORDEN");
		if (numOrdenes == 0) {
			cout << "  No hay ordenes." << endl;
			return;
		}
		listarOrdenes();
		int idO = leerEntero("\n  ID de la orden: ");
		if (idO < 1 || idO > numOrdenes) {
			cout << "  Orden inexistente." << endl;
			return;
		}
		trazarDistancia(ordenes[idO - 1]);
		mostrarResumen(ordenes[idO - 1]);
		cout << endl;
		ordenes[idO - 1].mostrarTrazado();
	}
	
	void verMisPedidos(int idC) {
		titulo("MIS PEDIDOS");
		int cantidad = 0;
		cout << endl;
		for (int i = 0; i < numOrdenes; i++) {
			if (ordenes[i].getIdCliente() == idC) {
				mostrarLineaOrden(i);
				cantidad++;
			}
		}
		if (cantidad == 0) {
			cout << "  Aun no tiene pedidos." << endl;
			return;
		}
		int idO = leerEntero("\n  ID del pedido para ver su trazado (0 = volver): ");
		if (idO == 0) return;
		if (idO < 1 || idO > numOrdenes || ordenes[idO - 1].getIdCliente() != idC) {
			cout << "  Pedido inexistente." << endl;
			return;
		}
		trazarDistancia(ordenes[idO - 1]);
		mostrarResumen(ordenes[idO - 1]);
		cout << endl;
		ordenes[idO - 1].mostrarTrazado();
	}
	
	void guardar() {
		ofstream f(ARCHIVO.c_str());
		if (!f) {
			cout << "  Error: no se pudo abrir " << ARCHIVO << endl;
			return;
		}
		f << setprecision(10);
		
		f << numTiendas << endl;
		for (int i = 0; i < numTiendas; i++) {
			f << tiendas[i].getId() << endl;
			f << tiendas[i].getNombre() << endl;
			f << tiendas[i].getUbicacion().getLatitud() << endl;
			f << tiendas[i].getUbicacion().getLongitud() << endl;
			f << tiendas[i].getNumItems() << endl;
			for (int j = 0; j < tiendas[i].getNumItems(); j++) {
				f << tiendas[i].getItem(j).nombre << endl;
				f << tiendas[i].getItem(j).precio << endl;
			}
		}
		
		f << numClientes << endl;
		for (int i = 0; i < numClientes; i++) {
			f << clientes[i].getId() << endl;
			f << clientes[i].getNombre() << endl;
			f << clientes[i].getUbicacion().getLatitud() << endl;
			f << clientes[i].getUbicacion().getLongitud() << endl;
		}
		
		f << numMensajeros << endl;
		for (int i = 0; i < numMensajeros; i++) {
			f << mensajeros[i].getId() << endl;
			f << mensajeros[i].getNombre() << endl;
			f << mensajeros[i].getVehiculo() << endl;
			f << mensajeros[i].getUbicacion().getLatitud() << endl;
			f << mensajeros[i].getUbicacion().getLongitud() << endl;
			if (mensajeros[i].estaDisponible()) f << 1 << endl;
			else f << 0 << endl;
		}
		
		f << numOrdenes << endl;
		for (int i = 0; i < numOrdenes; i++) {
			f << ordenes[i].getId() << endl;
			f << ordenes[i].getIdTienda() << endl;
			f << ordenes[i].getIdCliente() << endl;
			f << ordenes[i].getIdMensajero() << endl;
			f << ordenes[i].getEstado() << endl;
			f << ordenes[i].getDistanciaKm() << endl;
			f << ordenes[i].getTarifa() << endl;
			f << ordenes[i].getNumLineas() << endl;
			for (int j = 0; j < ordenes[i].getNumLineas(); j++) {
				f << ordenes[i].getLinea(j).nombre << endl;
				f << ordenes[i].getLinea(j).precio << endl;
				f << ordenes[i].getLinea(j).cantidad << endl;
			}
		}
		f.close();
	}
	
	bool cargar() {
		ifstream f(ARCHIVO.c_str());
		if (!f) return false;
		
		numTiendas = atoi(leerLinea(f).c_str());
		for (int i = 0; i < numTiendas; i++) {
			int id = atoi(leerLinea(f).c_str());
			string nombre = leerLinea(f);
			double lat = atof(leerLinea(f).c_str());
			double lon = atof(leerLinea(f).c_str());
			tiendas[i] = Tienda(id, nombre, Ubicacion(lat, lon));
			int cantItems = atoi(leerLinea(f).c_str());
			for (int j = 0; j < cantItems; j++) {
				string nombreItem = leerLinea(f);
				double precio = atof(leerLinea(f).c_str());
				tiendas[i].agregarItem(nombreItem, precio);
			}
		}
		
		numClientes = atoi(leerLinea(f).c_str());
		for (int i = 0; i < numClientes; i++) {
			int id = atoi(leerLinea(f).c_str());
			string nombre = leerLinea(f);
			double lat = atof(leerLinea(f).c_str());
			double lon = atof(leerLinea(f).c_str());
			clientes[i] = Cliente(id, nombre, Ubicacion(lat, lon));
		}
		
		numMensajeros = atoi(leerLinea(f).c_str());
		for (int i = 0; i < numMensajeros; i++) {
			int id = atoi(leerLinea(f).c_str());
			string nombre = leerLinea(f);
			string vehiculo = leerLinea(f);
			double lat = atof(leerLinea(f).c_str());
			double lon = atof(leerLinea(f).c_str());
			int disp = atoi(leerLinea(f).c_str());
			mensajeros[i] = Mensajero(id, nombre, vehiculo, Ubicacion(lat, lon), disp == 1);
		}
		
		numOrdenes = atoi(leerLinea(f).c_str());
		for (int i = 0; i < numOrdenes; i++) {
			int id = atoi(leerLinea(f).c_str());
			int idT = atoi(leerLinea(f).c_str());
			int idC = atoi(leerLinea(f).c_str());
			int idM = atoi(leerLinea(f).c_str());
			int estado = atoi(leerLinea(f).c_str());
			double dist = atof(leerLinea(f).c_str());
			double tarifa = atof(leerLinea(f).c_str());
			ordenes[i] = Orden(id, idT, idC, idM, estado, dist, tarifa);
			int cantLineas = atoi(leerLinea(f).c_str());
			for (int j = 0; j < cantLineas; j++) {
				string nombre = leerLinea(f);
				double precio = atof(leerLinea(f).c_str());
				int cantidad = atoi(leerLinea(f).c_str());
				ordenes[i].agregarLinea(nombre, precio, cantidad);
			}
		}
		f.close();
		return true;
	}
};

void menuEntidades(SistemaDelivery &s) {
	int op;
	do {
		limpiarPantalla();
		titulo("GESTION DE ENTIDADES");
		cout << "  1. Registrar tienda/restaurante" << endl;
		cout << "  2. Registrar mensajero" << endl;
		cout << "  3. Ver tiendas" << endl;
		cout << "  4. Ver clientes" << endl;
		cout << "  5. Ver mensajeros" << endl;
		cout << "  0. Volver" << endl;
		op = leerEntero("\n  Opcion: ");
		if (op == 0) break;
		limpiarPantalla();
		switch (op) {
			case 1: s.registrarTienda(); break;
			case 2: s.registrarMensajero(); break;
			case 3: s.verTiendas(); break;
			case 4: s.verClientes(); break;
			case 5: s.verMensajeros(); break;
			default: cout << "  Opcion invalida." << endl;
		}
		pausa();
	} while (op != 0);
}

void menuSeguimiento(SistemaDelivery &s) {
	int op;
	do {
		limpiarPantalla();
		titulo("ASIGNACION Y SEGUIMIENTO");
		cout << "  1. Asignar mensajero mas cercano" << endl;
		cout << "  2. Avanzar estado de una orden" << endl;
		cout << "  3. Ver ordenes y trazado" << endl;
		cout << "  0. Volver" << endl;
		op = leerEntero("\n  Opcion: ");
		if (op == 0) break;
		limpiarPantalla();
		switch (op) {
			case 1: s.asignarMensajero(); break;
			case 2: s.avanzarOrden(); break;
			case 3: s.verTrazadoOrden(); break;
			default: cout << "  Opcion invalida." << endl;
		}
		pausa();
	} while (op != 0);
}

void menuAlmacenamiento(SistemaDelivery &s) {
	int op;
	do {
		limpiarPantalla();
		titulo("ALMACENAMIENTO(ARCHIVO)");
		cout << "  1. Guardar en " << ARCHIVO << endl;
		cout << "  2. Leer desde " << ARCHIVO << endl;
		cout << "  0. Volver" << endl;
		op = leerEntero("\n  Opcion: ");
		if (op == 0) break;
		limpiarPantalla();
		switch (op) {
			case 1:
				s.guardar();
				cout << "  Datos guardados en " << ARCHIVO << endl;
				break;
			case 2:
				if (s.cargar()) cout << "  Datos leidos desde " << ARCHIVO << endl;
				else cout << "  Aun no existe el archivo " << ARCHIVO << endl;
				break;
			default: cout << "  Opcion invalida." << endl;
		}
		pausa();
	} while (op != 0);
}

void menuAdministrador(SistemaDelivery &s) {
	int op;
	do {
		limpiarPantalla();
		titulo("ADMINISTRADOR");
		cout << "  1. Gestion de entidades" << endl;
		cout << "  2. Asignacion y seguimiento" << endl;
		cout << "  3. Almacenamiento (archivo)" << endl;
		cout << "  0. Volver" << endl;
		op = leerEntero("\n  Opcion: ");
		switch (op) {
			case 1: menuEntidades(s); break;
			case 2: menuSeguimiento(s); break;
			case 3: menuAlmacenamiento(s); break;
			case 0: break;
			default:
				cout << "  Opcion invalida." << endl;
				pausa();
		}
	} while (op != 0);
}

void sesionCliente(SistemaDelivery &s, int idC) {
	int op;
	do {
		limpiarPantalla();
		titulo("CLIENTE: " + s.nombreCliente(idC));
		cout << "  1. Ver menu de una tienda" << endl;
		cout << "  2. Hacer un pedido" << endl;
		cout << "  3. Ver mis pedidos y su trazado" << endl;
		cout << "  0. Cerrar sesion" << endl;
		op = leerEntero("\n  Opcion: ");
		if (op == 0) break;
		limpiarPantalla();
		switch (op) {
			case 1: s.verMenu(); break;
			case 2: s.crearOrden(idC); break;
			case 3: s.verMisPedidos(idC); break;
			default: cout << "  Opcion invalida." << endl;
		}
		pausa();
	} while (op != 0);
}

void menuCliente(SistemaDelivery &s) {
	int op;
	do {
		limpiarPantalla();
		titulo("CLIENTE");
		cout << "  1. Registrarme" << endl;
		cout << "  2. Ingresar con mi ID" << endl;
		cout << "  0. Volver" << endl;
		op = leerEntero("\n  Opcion: ");
		if (op == 0) break;
		limpiarPantalla();
		if (op == 1) {
			int id = s.registrarCliente();
			pausa();
			if (id != 0) sesionCliente(s, id);
		} else if (op == 2) {
			titulo("INGRESAR");
			int id = leerEntero("  Su ID de cliente: ");
			if (s.existeCliente(id)) {
				sesionCliente(s, id);
			} else {
				cout << "  ID no registrado." << endl;
				pausa();
			}
		} else {
			cout << "  Opcion invalida." << endl;
			pausa();
		}
	} while (op != 0);
}

int main() {
	cout << fixed << setprecision(2);
	
	SistemaDelivery sistema;
	sistema.cargar();
	
	int op;
	do {
		limpiarPantalla();
		titulo("DELIVERY PUNO");
		cout << "  1. Administrador" << endl;
		cout << "  2. Cliente" << endl;
		cout << "  0. Salir" << endl;
		op = leerEntero("\n  Opcion: ");
		
		switch (op) {
			case 1: menuAdministrador(sistema); break;
			case 2: menuCliente(sistema); break;
			case 0:
				sistema.guardar();
				cout << "\n  Datos guardados. Hasta luego." << endl;
				break;
			default:
				cout << "  Opcion invalida." << endl;
				pausa();
		}
	} while (op != 0);
	
	return 0;
}