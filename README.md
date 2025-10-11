# BoundDB
BoundDB isn't a programming genius, but rather a concept for storing data and retrieving it faster and more accurately.


**Introduction**
To use you will need first add init.cpp in a folder and create a folder called "bound" and a subfolder called "db" and another subfolder in db called "tables"

It's very simple to compile you need inside repository use the command "g++ init.cpp -o bound"

**Example from files on table**
find.bd
<|>md201>|<0>|<0>|<identifier<!>another_identifier<!><|>md201>|<0>|<1>|<wow_identifier<!><|>

config.bd
{"currensm":"md201"<|>"qty":"0"<|>"indza":"2"<|>"sizeKb":"0"<|>"identifier":user<!><|>}

md201/0.bd
<|>name_identifier:^:identifier>|<name_another_identifier:^:another_identifier<|>name_identifier:^:wow_identifier<|>

**Some Commands**

1. bound create_table <NameTable> <[name_identifier0...]>
2. bound insert table_name
   To use it you will need add a insert.bd with necessary parameters e.g name_identifier0:^:Rafael>|<age:^:22
3. bound find <NameTable> <name_identifier>
4. bound update <NameTable> <Identification> <existent_parameter1:^:newdata1>|<new_parameter1:^:newdata2...>
5. bound delete <NameTable> <Identification>

You can see all the commands in the source, there are many more rules than those mentioned, I intend to explain some more specific things over time.

**Notes**
Peoples ever sanitize it if you'll use to communicate with a client.
I recommend using this when you need to store a lot of data and need few identifiers for that data. This database has specific rules that, if broken, can break all the data, but I use it daily, as it prevents several bugs. If you find one, you can notify other users and help fix it.

**Dogecoin address**
DAXqaA5XDMBEiDZai8jXV4drkLGS4m9MXM
