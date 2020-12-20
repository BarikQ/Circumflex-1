#include "Map.h"

Map::Map(const QJsonObject &staticObj, const QJsonObject &dynamicObj, const QJsonObject &pointsCoords, Player& player) {
    if (!dynamicObj.contains("trains") || !dynamicObj["trains"].isArray() || !dynamicObj.contains("posts") || !dynamicObj["posts"].isArray() || !dynamicObj.contains("idx"))
        throw std::invalid_argument("Wrong JSON graph format.");

    QJsonArray trainsJsonArray = dynamicObj["trains"].toArray();
    QJsonArray postsJsonArray = dynamicObj["posts"].toArray();

    for (auto const &post : postsJsonArray) {
        if (!post.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");

        int k = post.toObject()["type"].toInt();
        switch(k){
        case 1:
            towns_.emplace_back(post.toObject());
            posts_.emplace_back(post.toObject());

            if(post.toObject()["player_idx"].toString() != nullptr) {
                if(post.toObject()["player_idx"].toString() == player.idx()) {
                    player.setTown(post.toObject());
                }
            }
            break;
        case 2:
            markets_.emplace_back(post.toObject());
            posts_.emplace_back(post.toObject());
            break;
        case 3:
            storages_.emplace_back(post.toObject());
            posts_.emplace_back(post.toObject());
            break;
        }
    }

    for (auto const &train : trainsJsonArray) {
        if (!train.isObject())
            throw std::invalid_argument("Wrong JSON graph format.");
        trains_.emplace_back(train.toObject());

        if(train.toObject()["player_idx"].toString() != nullptr) {
            if(train.toObject()["player_idx"].toString() == player.idx()) {
                player.setTrain(train.toObject());
            }
        }
    }

    idx_ = dynamicObj["idx"].toInt();

    graph_ = Graph(staticObj, *this);
    graph_.calcCoords(16.0f / 9, pointsCoords);
}

Graph &Map::graph() {
    return graph_;
}

std::vector<Town>& Map::towns(){
    return towns_;
}

std::vector<Market>& Map::markets(){
    return markets_;
}

std::vector<Storage>& Map::storages(){
    return storages_;
}

std::vector<Post>& Map::posts(){
    return posts_;
}

void Map::makeWays(){
    int n = graph_.vertices().size();
    std::vector<std::vector<int>> mas(n);
    std::vector<std::vector<int>> p(n);
    int INF = 100000200;

    for(int t = 0; t < trains_.size(); ++t){
        for (int i = 0 ; i < n; ++i){
            mas[i] = std::vector<int>(n, INF);
            p[i] = std::vector<int>(n, INF);
            mas[i][i] = 0;
            p[i][i] = 0;
        }
        for(int i = 0 ; i < n;++i){
            int u =  graph_.vertices()[i].idx();
            for(int j = 0; j < graph_.vertices()[i].edges().size(); ++j){
                if(u == graph_.vertices()[i].edges()[j].get().vertex1().idx()){
                    mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                }
                else{
                    mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                        [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                        graph_.vertices()[i].edges()[j].get().length();
                }
               if( graph_.vertices()[i].edges()[j].get().vertex1().isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[i].edges()[j].get().vertex1().post().type()) == 3){
                        mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                            -1;
                        mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                            -1;
                    }
                }
                if( graph_.vertices()[i].edges()[j].get().vertex2().isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[i].edges()[j].get().vertex2().post().type()) == 3){
                        mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())] =
                            -1;
                        mas[graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex1().idx())]
                            [graph_.idx().at(graph_.vertices()[i].edges()[j].get().vertex2().idx())] =
                            -1;
                    }
                }
            }
        }
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n;++j){
                p[i][j] = mas[i][j];
                if(mas[i][j] != 0 && mas[i][j] != INF && mas[i][j] != -1){
                    p[i][j] = j;
                }
                if(mas[i][j] == INF && graph_.vertices()[i].isPostIdxNull() == false){
                   if(static_cast<int>(graph_.vertices()[i].post().type()) == 3){
                        mas[i][j] = -1;
                        p[i][j] = -1;
                    }
                }
                if(mas[i][j] == INF && graph_.vertices()[j].isPostIdxNull() == false){
                    if(static_cast<int>(graph_.vertices()[j].post().type()) == 3){
                         mas[i][j] = -1;
                         p[i][j] = -1;
                     }
                }
            }
        }
        for(int k = 0; k < n; ++k){
            for(int i = 0; i < n; ++i){
                for(int j = 0; j < n; ++j){
                    if(mas[i][j] != -1 && mas[i][k] != -1 && mas[k][j] != -1){
                        if(mas[i][j] > mas[i][k] + mas[k][j]){
                                p[i][j] = p[i][k];
                        }
                        mas[i][j] = std::min(mas[i][j], mas[i][k] + mas[k][j]);
                    }
                }
            }
        }
        trains_[t].trainWays(mas,p);
    }
}

std::vector<Train>& Map::trains(){
    return trains_;
}


