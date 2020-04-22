# Graphic

## RUS
Реализация собственного графического движка на C++.\
**Цели**
1. Отрисовать 3D сцены.
2. Повысить производительность, используя векторные интрукции.(Бенчмарк показал, что, благодаря компиляторной оптимизации, векторные инструкции не улучшают производительность, а в некоторых случаях ухудшают, например, когда не срабатывает RVO. Поэтому их **не будем применять**.)
3. Повысить производительность, используя многопоточность.
1ая попытка распараллелить вычисления была почти успешной. Добились увеличения производительности в 3 раза на 6-ти ядерном компьютере. 
**IPC** уменьшились в 2.3 раза (было 1.6, стало 0.7). 
**cache-miss rate** уменьшился в 2.9 раза (было 26%, стало 9%).

3. Отказаться от вычислений с большой областью захвата, чтобы уменьшить подгрузки кэш-линий в в соседнии процессора. Избажеать критических секций. Возможно, отказаться от общего thread_pool и сделать его более узконаправленным, но эффективным.

## ENG
Implementation of own graphics engine in C++. \
**Purposes**
1. To render a 3D scene.
2. Improve performance using vector instructions.(The benchmark showed that, because of compiler optimization, vector instructions do not improve performance, and in some cases it was getting worse, for example, when RVO did not work. Therefore, we will **not use it**.)
3. Improve performance using multithreading.

Current status:
![](https://github.com/BruceGliff/Graphic/blob/master/BG.png "BruceGliff")
