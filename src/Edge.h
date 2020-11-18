#ifndef LINE_H
#define LINE_H

#include "Vertex.h"
#include <QJsonObject>
#include <QMap>

class Edge{
    public:
        explicit Edge(const QJsonObject &edge, const QMap<int, Vertex*> &vertices);
        explicit Edge(int idx, int length, Vertex &v1, Vertex &v2);

        int idx();
        int length();
        Vertex& vertex1();
        Vertex& vertex2();

    private:
        int idx_;
        int length_;
        Vertex *vertex1_;
        Vertex *vertex2_;
};
#endif // LINE_H