#include<sys/types.h>
#include<iostream>
#include<vector>
#include<string>
#include<fcntl.h>
#include <cstdlib>
#include <stdexcept>

#define MAX_DELIMS_CNT  10
#define MAX_ROWS_CNT  10
#define MAX_COLUMS_CNT 10
#define MAX_BUFFER_SIZE 4


std::string convert_c_str_to_std_str(char buffer[]){
	std::string res;
	int len = strlen(buffer) - 1;
	for (int i = 0; i < len; i++){
		res.push_back(buffer[i]);
	}
	return res;
}



std::vector<std::string> tokenize(std::string raw){
	std::string current_string;
	std::vector<std::string> res;
	for (int i = 0; i < raw.size(); i++){
		if (raw[i] == ' '){
			if (current_string.size() > 0){
				res.push_back(current_string);
				res.push_back(" ");
				current_string = "";
			}
		}else if (raw[i] == '\n'){
			res.push_back(current_string);
			res.push_back("\n");
			current_string = "";
		}else if (raw[i] >= '0' && raw[i] <= '9'){
			current_string.push_back(raw[i]);
		}else{
			throw std::runtime_error("unsupported symbol in input");
		}
	}
	if (current_string.size() > 0){
		res.push_back(current_string);
	}
	return res;
}


std::vector<std::vector<int> > read_matrix(std::string filename){
	std::vector<std::vector<int> > result;
	std::vector<int> current_row;
	int f = open(filename.c_str(), O_RDONLY, S_IREAD);
	
	char buffer[MAX_BUFFER_SIZE];
	int to_read = MAX_BUFFER_SIZE;
	int readed = 0;
	std::string carry;
	while ((readed = read(f, buffer, to_read)) > 0) {
		std::string readed_str = convert_c_str_to_std_str(buffer);
		std::vector<std::string > tokens = tokenize(carry + readed_str);
		if (tokens.size() == 0){
			//todo
			break;
		}
		if (!(tokens[tokens.size() - 1] == " " || tokens[tokens.size() - 1] == "\n")){
			carry = tokens[tokens.size() - 1];
			tokens.erase(tokens.begin() + tokens.size() - 1);
		}
		for (int i = 0; i < tokens.size(); i++){
			if (tokens[i] == " "){
				continue;
			}else
			if (tokens[i] == "\n"){
				result.push_back(current_row);
				current_row.clear();
			}else{
				int tmp = atoi(tokens[i].c_str());
				current_row.push_back(tmp);
			}
		}
	}
	if (readed < 0){
		throw std::runtime_error("problems with reading matrix");
	}

	if (result.size() == 0){
		throw std::runtime_error("problems with reading matrix");
	}

	int n = result[0].size();
	for (int i = 0; i < result.size(); i++){
		if (result[i].size() != n){
			throw std::runtime_error("bad matrix in input");
		}
	}
	close(f);
	return result;
}



std::string serialize_row(std::vector<int> row){
	std::string res;
	for (int i = 0; i < row.size(); i++){
		char s[1024];
		int a = row[i];
		sprintf(s, "%d", a);
		std::string tmp(s);
		res = res + "["+ tmp + "] ";
	}
	res.push_back('\n');
	return res;
}

void write_matrix(std::vector<std::vector<int> > matrix){
	for (int i = 0 ; i < matrix.size(); i++){
		std::string s = serialize_row(matrix[i]);
		int writed = write(2, s.c_str(), s.size());
		if (writed < 0){
			throw std::runtime_error("problems with writing matrix");
		}
	}
	write(2, "\n", 1);
}

std::vector<std::vector<int> > multiple_matrix(std::vector<std::vector<int> > a, std::vector<std::vector<int> > b){
	if (a[0].size() != b.size()) {
		throw std::runtime_error("matrices are bad");
	}
	std::vector<std::vector<int> > result(a.size(), std::vector<int>(b[0].size()));
	for (int i = 0; i < result.size(); ++i) {
		for (int j = 0; j < result[i].size(); ++j) {
			for (int k = 0; k < b.size(); ++k) {
				result[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	return result;
}

int main(int argc, char** argv)
{
	if (argc < 3){
		return 1;
	}
	std::string file_name_one(argv[1]);
	std::string file_name_two(argv[2]);
	
	
	std::vector<std::vector<int> > matrix_a = read_matrix(file_name_one);
	std::vector<std::vector<int> > matrix_b = read_matrix(file_name_two);
	
	write_matrix(matrix_a);
	write_matrix(matrix_b);
	std::vector<std::vector<int> > c = multiple_matrix(matrix_a, matrix_b);
	write_matrix(c);
	
	return 0;
}
