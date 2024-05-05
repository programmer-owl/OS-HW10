# Макаревич Мария, БПИ228
Домашнее задание 10 по дисциплине "Операционные системы". Работа выполнена на **10 баллов**.
## Запуск программы
1. Запуск сервера:

   gcc server.c -o server
   ./server <IP-address> <Send Port> <Receive Port>
   

3. Запуск первого клиента:

   gcc client1.c -o client1
   ./client1 <Server IP> <Server Port>

3. Запуск второго клиента:

   gcc client2.c -o client2
   ./client2 <Server IP> <Server Port>
## Работа программы
Клиент №1 передает сообщения серверу, который перенаправляет их клиенту №2.
