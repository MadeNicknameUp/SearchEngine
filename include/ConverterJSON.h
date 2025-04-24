#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using json = nlohmann::json;

/*
* Класс для работы с JSON-файлами
*/
class ConverterJSON {
public:
    ConverterJSON() = default;

    /*
    * Метод получения содержимого файлов
    * @return Возвращает список с содержимым файлов перечисленных
    *
    в config.json
    */
    std::vector<std::string> GetTextDocuments() const;

    /*
    * Метод считывает поле max_responses для определения предельного
    * количества ответов на один запрос
    * @return
    */
    int GetResponsesLimit() const;

    /*
    * Метод получения запросов из файла requests.json
    * @return возвращает список запросов из файла requests.json
    */
    std::vector<std::string> GetRequests() const;

    /*
    * Положить в файл answers.json результаты поисковых запросов
    */
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answer) const;
};

#endif // CONVERTERJSON_H
