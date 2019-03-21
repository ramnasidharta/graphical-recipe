#include <gtk/gtk.h>
#include <string>

#include "drawer.hpp"

#ifndef VIEW_HPP
#define VIEW_HPP

using namespace std;

class View {

private:
  GtkBuilder *builder;

  GtkWidget *window;
  GtkWidget *newObjectWindow;
  GtkWidget *drawAreaViewPort;

  GtkEntry *entryPontoX;
  GtkEntry *entryPontoY;
  GtkEntry *objectName;

  GtkEntry *entryLineX;
  GtkEntry *entryLineY;
  GtkEntry *entryLineX1;
  GtkEntry *entryLineY1;
  GtkEntry *entryPolygonX;
  GtkEntry *entryPolygonY;

  GtkListBox *listObjects;
  GtkListBox *listCoordPolygon;

  GtkNotebook *notebookObjects;

  Drawer* drawer;
public:
  View() {
    drawer = new Drawer();
  }

  void initializeWindow(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "windows.glade", NULL); // TODO ATUALIZAR LOCAL DIRETORIO

    window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
    newObjectWindow = GTK_WIDGET(gtk_builder_get_object(builder, "windowInserirCoord"));
    drawAreaViewPort = GTK_WIDGET(gtk_builder_get_object(builder, "drawAreaViewPort"));

    entryPontoX = GTK_ENTRY(gtk_builder_get_object(builder, "entryPontoX"));
    entryPontoY = GTK_ENTRY(gtk_builder_get_object(builder, "entryPontoY"));
    objectName = GTK_ENTRY(gtk_builder_get_object(builder, "entryNovoObjeto"));
    entryLineX = GTK_ENTRY(gtk_builder_get_object(builder, "entryRetaX"));
    entryLineY = GTK_ENTRY(gtk_builder_get_object(builder, "entryRetaY"));
    entryLineX1 = GTK_ENTRY(gtk_builder_get_object(builder, "entryRetaX1"));
    entryLineY1 = GTK_ENTRY(gtk_builder_get_object(builder, "entryRetaY1"));
    entryPolygonX = GTK_ENTRY(gtk_builder_get_object(builder, "entryPoligonoX"));
    entryPolygonY = GTK_ENTRY(gtk_builder_get_object(builder, "entryPoligonoY"));

    listObjects = GTK_LIST_BOX(gtk_builder_get_object(builder, "listaObjetos"));
    listCoordPolygon = GTK_LIST_BOX(gtk_builder_get_object(builder, "listbox2"));

    notebookObjects = GTK_NOTEBOOK(gtk_builder_get_object(GTK_BUILDER(builder), "notebookObjects"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(G_OBJECT(builder));
    gtk_widget_show(window);
    gtk_main();
  }

  ~View() {}

  void create_surface(GtkWidget *widget) {
    drawer->create_surface(widget);
  }

  void draw(cairo_t *cr) {
    drawer->draw(cr);
  }

  void openNewObjectWindow() {
    gtk_widget_show(newObjectWindow);
  }

  void drawNewPoint(GraphicObject* obj) {
    drawer->drawPoint(obj->getCoordenada());
    gtk_widget_queue_draw((GtkWidget*) drawAreaViewPort);
  }

  void drawNewLine(GraphicObject* obj) {
    drawer->drawLine(obj->getCoordenadaIn(), obj->getCoordenadaFin());
    gtk_widget_queue_draw((GtkWidget*) drawAreaViewPort);
  }

  void drawNewPolygon(GraphicObject* obj) {
    vector<Coordenada*> polygonPoints = obj->getPolygonPoints();
    vector<Coordenada*>::iterator it;
    for(it = polygonPoints.begin(); it != polygonPoints.end()-1; it++) {
        drawer->drawLine(*it , *(std::next(it,1)));
    }
    gtk_widget_queue_draw((GtkWidget*) drawAreaViewPort);
  }

  void insertList(GraphicObject* obj, string tipo) {
    GtkWidget* row = gtk_list_box_row_new();
    GtkWidget* label = gtk_label_new((obj->getObjectName() + " ( " + tipo + " )").c_str());

    gtk_container_add((GtkContainer*) listObjects, label);
    gtk_widget_show_all((GtkWidget*) listObjects);
  }

  int removeFromList() {
    GtkListBoxRow* row = gtk_list_box_get_selected_row(listObjects);
    int index = gtk_list_box_row_get_index(row);

    gtk_container_remove((GtkContainer*) listObjects, (GtkWidget*) row);
    return index;
  }

  void insertCoordPolygonList() {
    string a = gtk_entry_get_text(entryPolygonX);
    string b = gtk_entry_get_text(entryPolygonY);
    string name = "Coordenada: (" + a + " , " + b + ")";

    GtkWidget* row = gtk_list_box_row_new();
    GtkWidget* label = gtk_label_new(name.c_str());

    gtk_container_add((GtkContainer*) listCoordPolygon, label);
    gtk_widget_show_all((GtkWidget*) listCoordPolygon);
  }

  int removeFromCoordPolygonList() {
    GtkListBoxRow* row = gtk_list_box_get_selected_row(listCoordPolygon);
    int index = gtk_list_box_row_get_index(row);

    gtk_container_remove((GtkContainer*) listCoordPolygon, (GtkWidget*) row);
    return index;
  }

  void clear_surface() {
    drawer->clear_surface();
    gtk_widget_queue_draw((GtkWidget*) drawAreaViewPort);
  }

  int getCurrentPage () {
    return gtk_notebook_get_current_page(notebookObjects);
  }

  // Gets
  double getEntryPontoX() {
    return stod(gtk_entry_get_text(entryPontoX));
  }

  double getEntryPontoY() {
    return stod(gtk_entry_get_text(entryPontoY));
  }

  string getObjectName() {
    return gtk_entry_get_text(objectName);
  }

  double getEntryLineX() {
    return stod(gtk_entry_get_text(entryLineX));
  }

  double getEntryLineY() {
    return stod(gtk_entry_get_text(entryLineY));
  }

  double getEntryLineX1() {
    return stod(gtk_entry_get_text(entryLineX1));
  }

  double getEntryLineY1() {
    return stod(gtk_entry_get_text(entryLineY1));
  }

  double getEntryPolygonX() {
    return stod(gtk_entry_get_text(entryPolygonX));
  }

  double getEntryPolygonY() {
    return stod(gtk_entry_get_text(entryPolygonY));
  }

};

#endif
