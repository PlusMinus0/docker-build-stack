#include<QApplication>
#include <QString>
#include <QScriptEngine>
#include <QSvgGenerator>
#include <QXmlName>
#include <QNetworkAccessManager>
#include <QWebEngineHttpRequest>


int main(int argc, char *argv[]){
	QApplication app(argc, argv);
	QString str = "Hello World!";
	QScriptEngine script_engine;
	QSvgGenerator svg_generator;
	QXmlName xml_name;
	QNetworkAccessManager network;
	QWebEngineHttpRequest request;
	return 0;
}
