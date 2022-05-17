# Лог попыток добавить ScaNN 

Для того чтобы добавить ScaNN в ClickHouse, эта библиотека была переписанна с Bazel на CMake [PR](https://github.com/Vector-Similarity-Search-for-ClickHouse/google-research/pull/2). Но т.к. в зависимостях существуют Tnsorflow, который не поддерживает сборку на CMake, а только на Bazel. Для обхода этой проблемы было решено добавить его как собранную либу.

### Как добавить tensorflow
Для начала надо привести используемую tensorflow версию protobuf, в соответствие версии используемой в ClickHouse. (изменения необходимо внести в файле `tensorflow/workspace2.bzl` в цели под названием `com_google_protobuf`). После этого, достаточно собрать с помощью Bazel цель `tensorflow_cc`. И прописать в cmake пути до собранных либ (лежат в директории `bazel-bin/tensorflow`) и хэдеров (необходимые директории `bazel-bin/tensorflow` и `third_party`).

Собранный таким образом ScaNN работает. Для использования этой библиотеки было написанно API для С++, т.к. существовало оно только для Python

Но при попытке добавить ScaNN в ClickHouse возникают проблемы, что ClickHouse не находит символы tensorflow. И решения этой проблемы найдено не было. Поэтому решенно было подтянуть символы руками анлогично [примеру](https://github.com/ClickHouse/ClickHouse/blob/788b704f7c153df1ede4e220214a3cda25c57918/src/Interpreters/CatBoostModel.cpp). Но символы было решено подтягивать отдельно собранного ScaNN, а не tensorflow, т.к. их там меньше =) Но сделать это успешно не удалось =( 

