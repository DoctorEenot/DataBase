#pragma once
#include <iostream>
#include <fstream>

struct Types{
	const char str[8] = "string\0";
	const char integer[9] = "integer\0";
	const char uinteger[10] = "uinteger\0";
	const char _float[7] = "float\0";
	const char _long[6] = "long\0";
	const char ulong[7] = "ulong\0";
	const char _double[8] = "double\0";
	const char boolean[9] = "boolean\0";

}types;

bool _false = false;

class Cell {
private:
	unsigned char* value;
	bool empty;

public:

	void set_value(unsigned char* value, char* type) {
		if (strcmp(type, types.str) == 0) {
			if (!empty) {
				delete[] this->value;
			}
			this->value = new unsigned char[strlen((char*)value) + 1];
			strcpy_s((char*)this->value, strlen((char*)value) + 1, (char*)value);
		}

		else if (strcmp(type, types.integer) == 0 || strcmp(type, types.uinteger) == 0 || strcmp(type, types._float) == 0) {
			if (empty) {
				this->value = new unsigned char[sizeof(int)];
			}
			memcpy(this->value, value, sizeof(int));
		}
		else if (strcmp(type, types._long) == 0 || strcmp(type, types.ulong) == 0 || strcmp(type, types._double) == 0) {
			if (empty) {
				this->value = new unsigned char[sizeof(long long)];
			}
			memcpy(this->value, value, sizeof(long long));
		}
		else if (strcmp(type, types.boolean) == 0) {
			if (empty) {
				this->value = new unsigned char[1];
			}
			memcpy(this->value, value, 1);
		}
		empty = false;
	}

	unsigned int get_size(char* type) {
		
		if (empty) {
			return 0;
		}

		if (strcmp(type, types.str) == 0) {
			return strlen((char*)value)+1;
		}

		else if (strcmp(type, types.integer) == 0 || strcmp(type, types.uinteger) == 0 || strcmp(type, types._float) == 0) {
			return sizeof(int);
		}
		else if (strcmp(type, types._long) == 0 || strcmp(type, types.ulong) == 0 || strcmp(type, types._double) == 0) {
			return sizeof(long long);
		}
		else if (strcmp(type, types.boolean) == 0) {
			return sizeof(bool);
		}
	}

	unsigned char* get_value() {
		return value;
	}

	bool is_empty() {
		return empty;
	}


	Cell(char* type, void* value) {

		this->set_value((unsigned char*)value, type);
		this->empty = false;

	}
	Cell(char* type) {

		this->empty = true;
	}
	Cell() {
		this->empty = true;
	}
	void clear() {
		if (!this->empty) {
			delete[] value;
			empty = true;
		}

	}
	

};

class Column {
private:
	char* name;
	char* type;

	unsigned int n_rows;
	Cell* cells;
	bool inited;
public:
	Column() {
		inited = false;
	}
	void init(char* name, char* type, unsigned int n_rows) {
		if (inited) {
			return;
		}
		this->name = new char[strlen(name) + 1];
		this->type = new char[strlen(type) + 1];
		strcpy_s(this->name, strlen(name) + 1, name);
		strcpy_s(this->type, strlen(type) + 1, type);
		this->n_rows = n_rows;

		this->cells = new Cell[this->n_rows];
		inited = true;
	}

	void set_name(char* name) {
		delete[] name;
		this->name = new char[strlen(name) + 1];
		strcpy_s(this->name, strlen(name) + 1, name);
	}

	unsigned int get_size(unsigned int row) {
		return cells[row].get_size(this->type);
	}

	bool is_inited() {
		return inited;
	}

	bool cell_empty(unsigned int cell) {
		return cells[cell].is_empty();
	}
	void clear() {
		for (unsigned int i = 0; i < n_rows; i++) {
			cells[i].clear();
		}
		delete[] cells;
		n_rows = 0;
	}
	void uninit() {
		if (!this->inited) {
			return;
		}
		delete[] name;
		delete[] type;
		this->clear();
		this->inited = false;
	}

	bool set_value(unsigned int cell, void* value) {
		if (cell >= n_rows || cell < 0) {
			return false;
		}
		cells[cell].set_value((unsigned char*)value, type);
		return false;
	}

	unsigned char* get_value(unsigned int cell) {
		if (cell >= n_rows || cell < 0) {
			return (unsigned char *)&_false;
		}
		return cells[cell].get_value();

	}

	void append_cell(unsigned int count) {

		Cell* buf = new Cell[this->n_rows + count];
		
		unsigned int counter = n_rows * sizeof(Cell);

		if (counter != 0) {
			memcpy(buf, cells, counter);
		}
		n_rows = n_rows + count;
		delete[] cells;
		cells = buf;
	}
	bool is_cell_empty(unsigned int cell) {
		return cells[cell].is_empty();
	}

	char* get_name() {
		return name;
	}
	char* get_type() {
		return type;
	}

	Cell* pop(unsigned int cell_number) {
		//Returns pointer to Cell, must be deleted
		Cell* to_return = new Cell();
		memcpy(to_return,&cells[cell_number],sizeof(Cell));				
		if (cell_number != (this->n_rows - 1)) {
			Cell* buf = new Cell[n_rows];
			memcpy(buf, cells, cell_number * sizeof(Cell));
			memcpy(&buf[cell_number], &cells[cell_number + 1], (n_rows - 1 - cell_number) * sizeof(Cell));
			delete[] cells;
			cells = buf;			
			
		}
		
		return to_return;
	}

	void insert(unsigned int cell_number, unsigned char* data) {
		Cell* buf = new Cell[n_rows + 1];
		memcpy(buf,cells,cell_number*sizeof(Cell));
		buf[cell_number].set_value(data,this->type);
		memcpy(&buf[cell_number+1],&cells[cell_number],(n_rows-cell_number)*sizeof(Cell));

		delete[] cells;
		cells = buf;
		n_rows += 1;
	}


	~Column(){
		
	}
};



class DB {
private:
	unsigned int count_of_rows;
	unsigned int count_of_columns;
	//unsigned int last_row=0;
	char* name;
	Column* columns;
	bool inited;
public:
	DB(char* name, unsigned int count_of_columns, unsigned int count_of_rows) {
		this->name = new char[strlen(name) + 1];
		strcpy_s(this->name, strlen(name) + 1, name);
		this->count_of_columns = count_of_columns;
		this->count_of_rows = count_of_rows;
		this->columns = new Column[this->count_of_columns];
		inited = true;
	}

	DB() {
		inited = false;
	}
	void init(char* name, unsigned int count_of_columns, unsigned int count_of_rows) {
		this->name = new char[strlen(name) + 1];
		strcpy_s(this->name, strlen(name) + 1, name);
		this->count_of_columns = count_of_columns;
		this->count_of_rows = count_of_rows;
		this->columns = new Column[this->count_of_columns];
		inited = true;
	}

	unsigned int get_column_number(char* column_name) {
		unsigned int column_number=0;
		for (column_number; column_number < count_of_columns; column_number++) {
			if (strcmp(column_name, columns[column_number].get_name()) == 0) {
				return column_number;
			}
		}
		throw 25;
	}

	unsigned int get_size(char* column_name, unsigned int row) {
		return this->get_size(get_column_number(column_name),row);
	}

	unsigned int get_size(unsigned int column, unsigned int row) {
		unsigned int sz = this->columns[column].get_size(row);
		return sz;
	}

	bool cell_is_empty(char* column_name, unsigned int row) {
		return cell_is_empty(get_column_number(column_name), row);
	}

	bool cell_is_empty(unsigned int column, unsigned int row) {
		
		return this->columns[column].is_cell_empty(row);
		
	}

	bool init_column(unsigned int column, char* name, char* type) {
		if (column < 0 || column >= count_of_columns) {
			return false;
		}
		this->columns[column].init(name, type, count_of_rows);
		return true;
	}
	unsigned char* get_value(unsigned int column, unsigned int row) {
		if (column < 0 || column >= count_of_columns) {
			return (unsigned char*)&_false;
		}
		return this->columns[column].get_value(row);
	}
	bool set_value(unsigned int column, unsigned int row, void* value) {
		if (column < 0 || column >= count_of_columns) {
			return false;
		}
		return this->columns[column].set_value(row, value);
	}

	bool set_value(char* column_name, unsigned int row, void* value) {
		return set_value(get_column_number(column_name),row,value);
	}

	void* get_value(char* column_name, unsigned int row) {
		return get_value(get_column_number(column_name),row);
	}

	char* get_type(unsigned int column_number) {
		return this->columns[column_number].get_type();
	}

	char* get_type(char* column_name) {
		/*for (unsigned int i = 0; i < count_of_columns; i++) {
			if (strcmp(column_name, columns[i].get_name()) == 0) {
				return this->columns[i].get_type();
			}
		}*/
		
		return this->get_type(get_column_number(column_name));
		
	}

	void append_column(char* column_name, char* type) {
		Column* buf = new Column[this->count_of_columns + 1];
		unsigned int counter = sizeof(Column) * count_of_columns;
		memcpy(buf, columns, counter);
		delete[] columns;
		buf[count_of_columns].init(column_name, type, count_of_rows);
		columns = buf;
		count_of_columns += 1;
	}
	void append_rows(unsigned int count) {
		for (unsigned int i = 0; i < count_of_columns; i++) {
			columns[i].append_cell(count);
		}
	}

	void clear_db() {
		//deletes all entries from all columns
		count_of_rows = 0;
		for (unsigned int i = 0; i < count_of_columns; i++) {
			columns[i].clear();
		}
	}

	void delete_db() {
		for (unsigned int i = 0; i < count_of_columns; i++) {
			columns[i].uninit();
		}
		delete[] name;
		delete[] columns;
	}

	void uninit_column(unsigned int column, char* name, char* type) {
		if (column < 0 || column >= count_of_columns) {
			return;
		}
		columns[column].uninit();
		columns[column].init(name, type, count_of_rows);

	}

	void delete_column(unsigned int column) {
		columns[column].uninit();
		if (column < 0 || column >= count_of_columns) {
			return;
		}

		Column* buf = new Column[this->count_of_columns - 1];
		unsigned int counter = column * sizeof(Column);

		if (counter > 0) {
			memcpy(buf, columns, counter);
		}
		counter = sizeof(Column) * (count_of_columns - column - 1);

		if (counter > 0) {
			memcpy(&buf[column], &columns[column + 1], counter);
		}
		delete[] columns;
		columns = buf;
		count_of_columns -= 1;
	}

	void delete_column(char* column_name) {
		delete_column(get_column_number(column_name));
	}

	unsigned int get_count_of_rows() {
		return count_of_rows;
	}

	unsigned int get_count_of_columns() {
		return count_of_columns;
	}

	bool dump(char* path) {
		std::ofstream file;
		char* filename = new char[strlen(name) + 6 + strlen(path)];
		memcpy(filename, path, strlen(path));
		memcpy(&filename[strlen(path)], name, strlen(name));
		filename[strlen(path) + strlen(name)] = '.';
		filename[strlen(path) + strlen(name) + 1] = 'd';
		filename[strlen(path) + strlen(name) + 2] = 'a';
		filename[strlen(path) + strlen(name) + 3] = 't';
		filename[strlen(path) + strlen(name) + 4] = 'a';
		filename[strlen(path) + strlen(name) + 5] = '\0';
		file.open(filename, std::ios::binary);
		bool* empty = new bool[1];
		if (!file.is_open()) {
			return false;
		}
		file << (unsigned char*)name << '\0';
		for (size_t i = 0; i < sizeof(count_of_columns); i++) {
			file << ((unsigned char*)& count_of_columns)[i];
		}
		for (size_t i = 0; i < sizeof(count_of_rows); i++) {
			file << ((unsigned char*)& count_of_rows)[i];
		}

		for (size_t i = 0; i < count_of_columns; i++) {
			file << (unsigned char)columns[i].is_inited();
			if (!columns[i].is_inited()) {
				continue;
			}
			file.write((char*)columns[i].get_name(), strlen(columns[i].get_name())+1);
			
			file.write((char*)columns[i].get_type(), strlen(columns[i].get_type()) + 1);
			for (size_t n = 0; n < count_of_rows; n++) {
				empty[0] = columns[i].cell_empty(n);
				file.write((char*)empty,1);
				if (empty[0]) {					
					continue;
				}
				
				file.write((const char*)get_value(i,n),get_size(i,n));

				/*if (strcmp(columns[i].get_type(), types.str) == 0) {
					file << columns[i].get_value(n) << '\0';

				}
				else if (strcmp(columns[i].get_type(), types.integer) == 0 || strcmp(columns[i].get_type(), types.uinteger) == 0 || strcmp(columns[i].get_type(), types._float) == 0) {
					file.write((char*)this->get_value(i, n), sizeof(int));
				}
				else if (strcmp(columns[i].get_type(), types._long) == 0 || strcmp(columns[i].get_type(), types.ulong) == 0 || strcmp(columns[i].get_type(), types._double) == 0) {
					file.write((char*)this->get_value(i, n), sizeof(long long));
				}
				else if (strcmp(columns[i].get_type(), types.boolean) == 0) {
					file << *this->get_value(i, n);
				}*/
			}

		}
		delete[] empty;
		//file << "\r\n\0";
		file.close();
		delete[] filename;
		return true;
	}

	bool is_inited() {
		return inited;
	}

	bool load(char* path) {
		if (inited) {
			return false;
		}
		std::ifstream file;
		file.open(path, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}

		char* ch = new char[1];//!!!!!!!!!!!!!!!!!!!!!!!
		unsigned int len = 0;
		ch[0] = 'a';
		while (ch[0] != '\0') {
			file.read(ch, 1);
			len++;
		}
		this->name = new char[len];//!!!!!!!!!!!!!!!!!!!!!!!
		file.seekg(0, std::ios_base::beg);
		file.read(name, len);

		file.read((char*)& count_of_columns, sizeof(count_of_columns));
		this->columns = new Column[this->count_of_columns];//!!!!!!!!!!!!!!!!!!!!!!!
		file.read((char*)& count_of_rows, sizeof(count_of_rows));
		bool column_inited;
		unsigned char* column_name;
		unsigned char* column_type;

		for (unsigned int i = 0; i < count_of_columns; i++) {
			file.read((char*)& column_inited, 1);
			if (!column_inited) {
				continue;
			}
			len = 0;
			ch[0] = 'a';
			while (ch[0] != '\0') {
				file.read(ch, 1);
				len++;
			}
			column_name = new unsigned char[len];//!!!!!!!!!!!!!!!!!!!!!!!
			file.seekg((unsigned long)file.tellg() - len, std::ios_base::beg);
			file.read((char*)column_name, len);

			len = 0;
			ch[0] = 'a';
			while (ch[0] != '\0') {
				file.read(ch, 1);
				len++;
			}
			column_type = new unsigned char[len];//!!!!!!!!!!!!!!!!!!!!!!!
			file.seekg((unsigned long)file.tellg() - len, std::ios_base::beg);
			file.read((char*)column_type, len);

			columns[i].init((char*)column_name, (char*)column_type, count_of_rows);

			delete[] column_name;
			delete[] column_type;
			for (unsigned int n = 0; n < count_of_rows; n++) {
				file.read((char*)& column_inited, 1);
				if (column_inited) {
					continue;
				}
				if (strcmp(columns[i].get_type(), types.str) == 0) {
					len = 0;
					ch[0] = 'a';
					while (ch[0] != '\0') {
						file.read(ch, 1);
						len++;
					}
					column_name = new unsigned char[len];//!!!!!!!!!!!!!!!!!!!!!!!
					file.seekg((unsigned long)file.tellg() - len, std::ios_base::beg);
					file.read((char*)column_name, len);

				}
				else if (strcmp(columns[i].get_type(), types.integer) == 0 || strcmp(columns[i].get_type(), types.uinteger) == 0 || strcmp(columns[i].get_type(), types._float) == 0) {
					column_name = new unsigned char[sizeof(int)];//!!!!!!!!!!!!!!!!!!!!!!!
					file.read((char*)column_name, sizeof(int));
				}
				else if (strcmp(columns[i].get_type(), types._long) == 0 || strcmp(columns[i].get_type(), types.ulong) == 0 || strcmp(columns[i].get_type(), types._double) == 0) {
					column_name = new unsigned char[sizeof(long long)];//!!!!!!!!!!!!!!!!!!!!!!!
					file.read((char*)column_name, sizeof(long long));
				}
				else if (strcmp(columns[i].get_type(), types.boolean) == 0) {
					column_name = new unsigned char[sizeof(bool)];//!!!!!!!!!!!!!!!!!!!!!!!
					file.read((char*)column_name, sizeof(bool));
				}
				columns[i].set_value(n, column_name);
				delete[] column_name;
			}

		}
		delete[] ch;
		inited = true;
	}

	unsigned int* wheren(char* column_name, unsigned char* data) {// return [count,...]

		unsigned int* return_buf;
		unsigned int items_count = 1;

		if (strcmp(get_type(column_name), types.str) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (strcmp((char*)get_value(column_name, i), (char*)data) == 0) {
					items_count += 1;
				}
			}
			
		}
		else if (strcmp(get_type(column_name), types.integer) == 0 || strcmp(get_type(column_name), types.uinteger) == 0 || strcmp(get_type(column_name), types._float) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (((unsigned char*)get_value(column_name, i))[0] == data[0] && ((unsigned char*)get_value(column_name, i))[1] == data[1] && ((unsigned char*)get_value(column_name, i))[2] == data[2] && ((unsigned char*)get_value(column_name, i))[3] == data[3]) {
					items_count += 1;					
				}
			}
			
		}
		else if (strcmp(get_type(column_name), types._long) == 0 || strcmp(get_type(column_name), types.ulong) == 0 || strcmp(get_type(column_name), types._double) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (((unsigned char*)get_value(column_name, i))[0] == data[0] && ((unsigned char*)get_value(column_name, i))[1] == data[1] && ((unsigned char*)get_value(column_name, i))[2] == data[2] && ((unsigned char*)get_value(column_name, i))[3] == data[3] && ((unsigned char*)get_value(column_name, i))[4] == data[4] && ((unsigned char*)get_value(column_name, i))[5] == data[5] && ((unsigned char*)get_value(column_name, i))[6] == data[6] && ((unsigned char*)get_value(column_name, i))[7] == data[7]) {
					items_count += 1;					
				}
			}
			
		}
		else if (strcmp(get_type(column_name), types.boolean) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (((unsigned char*)get_value(column_name, i))[0] == data[0]) {
					items_count += 1;
				}
			}
			
		}
		
		return_buf = new unsigned int[items_count];
		return_buf[0] = items_count - 1;
		unsigned int spec_count = 1;
		if (items_count == 1) {
			return return_buf;
		}

		

	}

	unsigned int where(char* column_name,unsigned char* data) {//returns row number
		unsigned int row = 0;
		if (strcmp(get_type(column_name),types.str) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (cell_is_empty(column_name, i)) {
					continue;
				}
				if (strcmp((char*)get_value(column_name, i), (char*)data) == 0) {
					row = i;
					break;
				}
			}
			if (cell_is_empty(column_name, 0) || (row == 0 && strcmp((char*)get_value(column_name, row), (char*)data) != 0)) {
				throw 25;
			}
		}
		else if (strcmp(get_type(column_name), types.integer) == 0 || strcmp(get_type(column_name), types.uinteger) == 0 || strcmp(get_type(column_name), types._float) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (((unsigned char*)get_value(column_name, i))[0] == data[0] && ((unsigned char*)get_value(column_name, i))[1] == data[1] && ((unsigned char*)get_value(column_name, i))[2] == data[2] && ((unsigned char*)get_value(column_name, i))[3] == data[3]) {
					row = i;
					break;
				}
			}
			if (row ==0 && (((unsigned char*)get_value(column_name, row))[0] != data[0] && ((unsigned char*)get_value(column_name, row))[1] != data[1] && ((unsigned char*)get_value(column_name, row))[2] != data[2] && ((unsigned char*)get_value(column_name, row))[3] != data[3])) {
				throw 25;
			}
		}
		else if (strcmp(get_type(column_name), types._long) == 0 || strcmp(get_type(column_name), types.ulong) == 0 || strcmp(get_type(column_name), types._double) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (((unsigned char*)get_value(column_name, i))[0] == data[0] && ((unsigned char*)get_value(column_name, i))[1] == data[1] && ((unsigned char*)get_value(column_name, i))[2] == data[2] && ((unsigned char*)get_value(column_name, i))[3] == data[3] && ((unsigned char*)get_value(column_name, i))[4] == data[4] && ((unsigned char*)get_value(column_name, i))[5] == data[5] && ((unsigned char*)get_value(column_name, i))[6] == data[6] && ((unsigned char*)get_value(column_name, i))[7] == data[7]) {
					row = i;
					break;
				}
			}
			if (row == 0 && (((unsigned char*)get_value(column_name, row))[0] != data[0] && ((unsigned char*)get_value(column_name, row))[1] != data[1] && ((unsigned char*)get_value(column_name, row))[2] != data[2] && ((unsigned char*)get_value(column_name, row))[3] != data[3] && ((unsigned char*)get_value(column_name, row))[4] != data[4] && ((unsigned char*)get_value(column_name, row))[5] != data[5] && ((unsigned char*)get_value(column_name, row))[6] != data[6] && ((unsigned char*)get_value(column_name, row))[7] != data[7])) {
				throw 25;
			}
		}
		else if (strcmp(get_type(column_name), types.boolean) == 0) {
			for (unsigned int i = 0; i < this->count_of_rows; i++) {
				if (((unsigned char*)get_value(column_name, i))[0] == data[0]) {
					row = i;
					break;
				}
			}
			if (row == 0 && (((unsigned char*)get_value(column_name, row))[0] != data[0] )) {
				throw 25;
			}
		}
		return row;
	}

	
	Cell* pop(unsigned int cell_number) {	
		Cell* to_return = new Cell[count_of_columns];
		Cell* temp;
		for (unsigned int i = 0; i < count_of_columns; i++) {
			temp = columns[i].pop(cell_number);
			memcpy(&to_return[i],temp,sizeof(Cell));
			delete temp;
		}
		
		return to_return;
		
	}


	void insert_row(unsigned int row_num, unsigned char* data) {
		unsigned int starting_offset = 0;
		for (unsigned int i = 0; i < count_of_columns; i++) {
			columns[i].insert(row_num, &data[starting_offset]);
			starting_offset += get_size(i,row_num);
		}
		count_of_rows += 1;
	}
	

	bool exist(char* column_name, unsigned char* data) {
		try {
			this->where(column_name,data);
		}
		catch(int e){
			return false;
		}
		return true;
	}

	char* get_name() {
		return name;
	}

	~DB() {
		
	}

};