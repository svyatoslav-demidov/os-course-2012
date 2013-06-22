#include<sys/types.h>
#include<iostream>
#include<vector>
#include<string>
#include<fcntl.h>

#define MAX_DELIMS_CNT  10
#define MAX_ROWS_CNT  10
#define MAX_COLUMS_CNT 10
#define MAX_BUFFER_SIZE 4

std::pair<int, int> get_pos_of_next_delimiter(std::string raw, std::vector<std::string> delimiter){
	int id = -1;
	int pos = -1;
	for (int i = 0; i < delimiter.size(); i++){
		int c_pos = raw.find(delimiter[i]);
		if ((c_pos < pos || pos == -1) && c_pos >= 0){
			pos = c_pos;
			id = i;
		}
	}
	return std::make_pair(pos, id);
}

bool is_int_contains_in_vector(int k, std::vector<int> v){
	for (int i = 0; i < v.size(); i++){
		if (k == v[i]){
			return true;
		}
	}
	return false;
}

std::pair<std::string, std::string> get_only_saved_colums(std::string raw, std::vector<std::string> d, std::vector<int> to_save){
	//int current_pos = 0;
	std::string result;
	while(true){
		std::pair<int, int> ps = get_pos_of_next_delimiter(raw, d);
		if (ps.first == -1){
			return std::make_pair(raw, result);
		}
		int id_d = ps.second;
		int pos_d = ps.first;
		if (is_int_contains_in_vector(id_d, to_save)){
			//copy column
			result = result + raw.substr(0, pos_d);
			//copy delim
			result = result + d[id_d];
		}
		raw = raw.substr(pos_d + d[id_d].size(), raw.length() - pos_d-d[id_d].size());
		
	}
	return std::make_pair(raw, result);
}

std::string convert_c_str_to_std_str(char buffer[]){
	std::string res;
	int len = strlen(buffer) - 1;
	for (int i = 0; i < len; i++){
		res.push_back(buffer[i]);
	}
	return res;
}

int main(int argc, char** argv)
{
	std::vector<std::string> delims;
	std::vector<int> colums_to_save;
	
	bool is_delims = true;
	for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], ";") != 0 && is_delims){
			delims.push_back(std::string(argv[i]));
			write(2, argv[i], strlen(argv[i]));
			write(2, "\n", 1);
		}else if (strcmp(argv[i],";") == 0){
			is_delims = false;
			write(2, "delims end\n", 11);
		}else{
			colums_to_save.push_back(atoi(argv[i]));
			write(2, "column to save: ", 16);
			write(2,argv[i],strlen(argv[i]));
			write(2, "\n", 1);
		}
	}
	char buffer[MAX_BUFFER_SIZE];
	int to_read = MAX_BUFFER_SIZE;
	int readed = 0;
	std::string carry;
	while ((readed = read(0, buffer, to_read)) > 0) {
		std::string readed_str = convert_c_str_to_std_str(buffer);
		//std::cout << "ololo" << readed_str;
		std::string to_process = carry + readed_str;
		std::pair<std::string, std::string> res = get_only_saved_colums(to_process, delims, colums_to_save);
		carry = res.first;
		std::string to_write = res.second;
		write(2, to_write.c_str(), to_write.size());
	}
	return 0;
}
