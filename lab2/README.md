# Лабораторная работа 2

**Название:** "Разработка драйверов блочных устройств"

**Цель работы:** Получение навыков разработки блочных устройств, создания дисков и деления их на разделы.

## Описание функциональности драйвера

Драйвер блочного устройства создает один первичный раздел размером 20Мбайт и один  расширенный раздел, содержащий три логических раздела  размером 10Мбайт каждый.

## Инструкция по сборке

1. Сборка проекта: `make`.

2. Добавление драйвера в ядро, создание файловых систем, создание точек монтирования `make do`

3. Команды для проверки:
  
- Проверка созданных дисков: `sudo fdisk -l /dev/mydisk`
  
- Сообщения логов в кольцевом бувере: `dmesg`
  
- Проверка созданных файловых систем: `df -h`

4. Удаление модуля из ядра и размонтирование файловых систем: `make rm` 

## Примеры использования

Проведем проверки после сборки программы и добавления ее в ядро системы. 

1. Вывод команды `sudo fdisk -l /dev/mydisk`:

![image](https://user-images.githubusercontent.com/27426287/110920689-bac2e080-832e-11eb-9500-e4b0deed1ef3.png)

2. Вывод команды `df -h` для проверки точек монтирования:

![image](https://user-images.githubusercontent.com/27426287/110920942-037a9980-832f-11eb-9d60-36269f62d861.png)

3. Сообщения в кольцевом буфере:

![image](https://user-images.githubusercontent.com/27426287/110922052-4ab55a00-8330-11eb-95f3-006ba9e35c4e.png)

## Профилирование

Копирование между разделами виртуального диска (из mydisk1 в mydisk6) -- 4.8 MB/s:

```bash
root@vagrant:~# dd if=/dev/mydisk1 of=/dev/mydisk6 bs=512 count=30720 oflag=direct
20480+0 records in
20479+0 records out
10485248 bytes (10 MB, 10 MiB) copied, 2.18378 s, 4.8 MB/s
```

Копирование из реального диска в виртуальный диск (из /dev/sda/ в mysidk6) -- 4.2 MB/s:

```bash
root@vagrant:~# dd if=/dev/sda of=/dev/mydisk6 bs=512 count=30720 oflag=direct
20480+0 records in
20479+0 records out
10485248 bytes (10 MB, 10 MiB) copied, 2.49536 s, 4.2 MB/s
```
