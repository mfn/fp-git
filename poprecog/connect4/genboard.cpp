/**
 * Connect Four AI by peter_k
 *
 * This single file is to create functions for quick board usage -> board.c
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

typedef struct sBoardSize {
	int width;
	int height;
} tBoardSize;

void setBit(unsigned long long &number, int bit) {
	unsigned long long tmp = 1;

	if (!((bit>=0)&&(bit<=63)))
	  return;
	while(bit--)
	  tmp*=2;
	number |= tmp;
}

int field(tBoardSize board, int y, int x) {
	return x+y*board.height;
}

int main(int argc, char *argv[])
{
	const int boardNumber = 1;
	const tBoardSize board[boardNumber] = {{4, 4}/*, {7, 6}, {7, 7}, {8, 7}, {8, 8}*/};
	ofstream output;
	int actualBoard;
	int column;
	int line;
	int height;
	int linex;
	int liney;
	int i;
	int firstRow;
	unsigned long long longNumber;
	unsigned long long longNumber2;
	int totalNumberOfRows;
	
	//longNumber=0;
	//setBit(longNumber, 2);
	//setBit(longNumber, 6);
	//setBit(longNumber, 10);
	//setBit(longNumber, 14);
	//cout << longNumber << endl;
	//system("pause");
	//return 0;
	
	output.open("boards.c");
	output
	<< "/**\n * Code below was generated by a machine, so please don't edit\n */\n\n"
	<< "class classBoard {\n"
	<< "	public:\n"
	<< "	unsigned long long board[2];\n"
	<< "	int actualPlayer;\n\n"
	<< "	inline void clear() {\n"
	<< "		board[0] = 0;\n		board[1] = 0;\n		actualPlayer = 0;\n"
	<< "	}\n";
	for (actualBoard=0; actualBoard<boardNumber; actualBoard++) {
		output
		<< "	int putToken" << board[actualBoard].width << board[actualBoard].height << "(int column);\n"
		<< "	int removeToken" << board[actualBoard].width << board[actualBoard].height << "(int column);\n";
	}
	output
	<< "};\n\n";
	
	/**
	 * putToken
	 */
	for (actualBoard=0; actualBoard<boardNumber; actualBoard++) {
    totalNumberOfRows=0;
		output
		<< "inline int classBoard::putToken" << board[actualBoard].width << board[actualBoard].height << "(int column) {\n"
		<< "	switch(column) {\n";
		for (column = 0; column < board[actualBoard].width; column++) {
			longNumber = 0;
			for (line=0; line<board[actualBoard].height; line++)
			  setBit(longNumber, field(board[actualBoard], line, column));
			output
			<< "		case " << column << ":\n"
			<< "			switch((board[0]|board[1])&" << longNumber << ") {\n";
			for (height=0; height<board[actualBoard].height; height++) {
				longNumber=0;
   			for (line=0; line<height; line++)
				  setBit(longNumber, field(board[actualBoard], line, column));
				longNumber2=0;
				setBit(longNumber2, field(board[actualBoard], height, column));
				output
				<< "				case " << longNumber << ":\n"
				<< "					board[actualPlayer]|=" << longNumber2 << ";\n"
				<< "					if (";
				firstRow=1;
				// poziome
				for (linex=column-3; linex<column+1; linex++)
				  if ((linex>=0)&&(linex<=board[actualBoard].width-4)) {
						longNumber2=0;
						for (i=0; i<4; i++)
						  setBit(longNumber2, field(board[actualBoard], height, linex+i));
						if (firstRow)
							firstRow=0;
						else
						  output
							<< " || \n					    ";
					  output
						<< "((board[actualPlayer]&" << longNumber2 << ")==" << longNumber2 << ")";
						totalNumberOfRows++;
					}
				// pionowe
				for (liney=height-3; liney<height+1; liney++)
				  if ((liney>=0)&&(liney<=board[actualBoard].height-4)) {
						longNumber2=0;
						for (i=0; i<4; i++)
						  setBit(longNumber2, field(board[actualBoard], liney+i, column));
						if (firstRow)
							firstRow=0;
						else
						  output
							<< " || \n					    ";
					  output
						<< "((board[actualPlayer]&" << longNumber2 << ")==" << longNumber2 << ")";
						totalNumberOfRows++;
					}
				// rosnace
				for (linex=column-3, liney=height-3; linex<column+1; linex++, liney++)
				  if ((linex>=0)&&(linex<=board[actualBoard].width-4)&&
					    (liney>=0)&&(liney<=board[actualBoard].height-4)&&
							(abs(linex-column)==abs(liney-height))) {
						longNumber2=0;
						for (i=0; i<4; i++)
						  setBit(longNumber2, field(board[actualBoard], linex+i, liney+i));
						if (firstRow)
							firstRow=0;
						else
						  output
							<< " || \n					    ";
					  output
						<< "((board[actualPlayer]&" << longNumber2 << ")==" << longNumber2 << ")";
						totalNumberOfRows++;
					}
				// malejace
				for (linex=column-3, liney=height; linex<column+1; linex++, liney--)
				  if ((linex>=0)&&(linex<=board[actualBoard].width-4)&&
					    (liney>=0)&&(liney<=board[actualBoard].height-4)&&
							(abs(linex-column)+abs(liney-height))==3) {
						longNumber2=0;
						for (i=0; i<4; i++)
						  setBit(longNumber2, field(board[actualBoard], linex+i, liney+3-i));
						if (firstRow)
							firstRow=0;
						else
						  output
							<< " || \n					    ";
					  output
						<< "((board[actualPlayer]&" << longNumber2 << ")==" << longNumber2 << ")";
						totalNumberOfRows++;
					}
				output
				<< ")\n"
				<< "						return 2;\n"
				<< "					break;\n";
			}
			output
			<< "				default:\n"
			<< "					return -2;\n"
			<< "					break;\n";
			output
			<< "			};\n"
			<< "			break;\n";
		}
		output
		<< "	};\n"
		<< "	actualPlayer=!actualPlayer;\n"
		<< "	// totalNumberOfRows=" << totalNumberOfRows/4 << "\n"
		<< "}\n\n";
	}
	
	/**
	 * removeToken
	 */
	for (actualBoard=0; actualBoard<boardNumber; actualBoard++) {
		output
		<< "inline int classBoard::removeToken" << board[actualBoard].width << board[actualBoard].height << "(int column) {\n"
		<< "	actualPlayer=!actualPlayer;\n"
		<< "	switch(column) {\n";
		for (column = 0; column < board[actualBoard].width; column++) {
			longNumber = 0;
			for (line=0; line<board[actualBoard].height; line++)
			  setBit(longNumber, field(board[actualBoard], line, column));
			output
			<< "		case " << column << ":\n"
			<< "			switch((board[0]|board[1])&" << longNumber << ") {\n";
			for (height=1; height<=board[actualBoard].height; height++) {
				longNumber=0;
   			for (line=0; line<height; line++)
				  setBit(longNumber, field(board[actualBoard], line, column));
				longNumber2=0;
				for (i=0; i<field(board[actualBoard], board[actualBoard].height, board[actualBoard].width); i++)
				  if (i != field(board[actualBoard], height-1, column))
					setBit(longNumber2, i);
				output
				<< "				case " << longNumber << ":\n"
				<< "					board[actualPlayer]&=" << longNumber2 << ";\n"
				<< "					break;\n";
			}
			/*output
			<< "				default:\n"
			<< "					return -2;\n"
			<< "					break;\n";*/
			output
			<< "			};\n"
			<< "			break;\n";
		}
		output
		<< "	};\n"
		<< "}\n\n";
	}
	
	output.close();

	//system("PAUSE");
	return EXIT_SUCCESS;
}
