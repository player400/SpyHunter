#pragma once

template<typename T>
class Vector
{
private:
	T* buffer = nullptr;
	int capacity;

public:
	int size;

	//FUNKCJA ZWRACA WSKAZNIK DO ZAWARTO??CI WEKTORA, ABY MOZNA BY?ÅO ODNIESC SIEò DO NIEGO POZA KLASA?
	T* get_ptr()
	{
		return buffer;
	}

	T get(int index)
	{
		return buffer[index];
	}

	//FUNKCJA SPRAWDZA CZY JEST JESZCZE MIEJSCE W WEKTORZE, JES?LI NIE DO POWIEKSZA GO DWUKROTNIE. NASTEòPNIE ZAPISUJE ELEMENT NA KONÉCU WEKTORA
	void push_back(T new_item)
	{
		if (size < capacity)
		{
			buffer[size] = new_item;
		}
		else
		{
			T* temp_buffer = (T*)malloc(sizeof(T) * capacity * 2);
			memcpy(temp_buffer, buffer, capacity * sizeof(T));
			temp_buffer[size] = new_item;
			capacity = capacity * 2;
			free(buffer);
			buffer = (T*)malloc(sizeof(T) * capacity);
			memcpy(buffer, temp_buffer, capacity * sizeof(T));
			free(temp_buffer);
		}
		size++;
	}

	//FUNKCJA EFEKTYWNIE USUWA OSTATNI ELEMENT WEKTORA (POD WARUNKIEM, ZE WEKTOR MA ROZMIAR WIEòKSZY OD 0)
	void pop_back()
	{
		if (size > 0)
		{
			size--;
		}
	}

	Vector()
	{
		size = 0;
		capacity = INIT_VECTOR_CAP;
		buffer = (T*)malloc(sizeof(T) * INIT_VECTOR_CAP);
	}

	~Vector()
	{
		free(buffer);
	}
};