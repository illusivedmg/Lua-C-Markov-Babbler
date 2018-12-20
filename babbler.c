#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <lua5.2/lua.h>
#include <lua5.2/lualib.h>
#include <lua5.2/lauxlib.h>

int readFile(lua_State *L) {
    const char *filename = lua_tostring(L, 1);
    char *buffer;
    int rc;
    long length;

    FILE *fin = fopen(filename, "rb");
    if(!fin) {
        perror("error opening file");
        exit(1);
    }

    rc = fseek(fin, 0, SEEK_END);
    assert(rc >= 0);

    length = ftell(fin);

    rc = fseek(fin, 0, SEEK_SET);
    assert(rc >= 0);

    // buffer for corpus and null-term
    buffer = malloc(length + 1);
    if(buffer == NULL) {
        fprintf(stderr, "error in malloc\n");
        exit(1);
    }

    fread(buffer, length, 1, fin);
    
    rc = fclose(fin);
    assert(rc >= 0);

    // null term for lua_pushstring
    buffer[length] = 0;
    lua_pushstring(L, buffer);

    lua_pushinteger(L, length);

    free(buffer);

    // success - returns # of args for lua (corpus, length)
    return 2;
}

// checks if token is proj gutenberg page num of format "12345m"
// moved to lua
// int isGutenbergPageNum(char *token, int len) {
//     if(token[len - 1] == 'm') {
//         for(int i = 0; i < len - 1; i++) {
//             if(!isdigit(token[i])) {
//                 return 0;
//             }
//         }
//         return 1;
//     }
//     return 0;
// }

int parseInput(lua_State *L) {
    const char *corpus = lua_tostring(L, 1);
    const int corpuslen = lua_rawlen(L, 1);
    const int offset = lua_tonumber(L, 2);

    char *rawtoken, *token;
    int start, end;
    size_t toklen;

    // finds token by splitting corpus by spaces and punctuation
    start = offset;
    while(start < corpuslen && (isspace(corpus[start]) || ispunct(corpus[start]))) {
        start++;
    }

    end = start;
    while(end < corpuslen && !(isspace(corpus[end]) || ispunct(corpus[end]))) {
        end++;
    }

    // create buffer to store raw token and processed token
    toklen = end - start;
    rawtoken = malloc(toklen);
    token = malloc(toklen);
    if(rawtoken == NULL || token == NULL) {
        fprintf(stderr, "error in malloc\n");
        exit(1);
    }

    for(int i = 0; i < toklen; i++) {
        rawtoken[i] = corpus[start + i];
    }

    // process raw token
    int j = 0;
    for(int i = 0; i < toklen; i++) {
        // check if alphanumeric
        // reason for mess - isalnum/iswalnum culls accented characters even in specified locales
        // && ispunct doesn't catch extended punctuation, e.g. left/right single/double quotes or long dashes
        if(!iscntrl(rawtoken[i])) {
            if(rawtoken[i] == -30 && i <= toklen - 2 && rawtoken[i + 1] == -128) {
                i = i + 2;
            } else {
                token[j] = tolower(rawtoken[i]);
                j++;
            }
        }
    }

    lua_pushlstring(L, token, j);
    lua_pushinteger(L, end); // new offset

    free(rawtoken);
    free(token);
    
    // success - passes # args to lua (token and new offset)
    return 2;
}

int main(int argc, char *argv[]) {
    if(argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: %s <filename> [words] [n]", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    int wordcount = 100, n = 3;

    if(argc == 3) {
        wordcount = atoi(argv[2]);
    }
    if(argc == 4) {
        n = atoi(argv[3]);
    }

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "readFile", readFile);
    lua_register(L, "parseInput", parseInput);
     
    char *luafile = "babbler.lua";
    // returns 0 for success
    if(luaL_dofile(L, luafile)) {
        printf("Error opening %s file\n", luafile);
		return 1;
    }

    lua_getglobal(L, "luamain");
    lua_pushstring(L, filename);
    lua_pushnumber(L, wordcount);
    lua_pushnumber(L, n);
    lua_pcall(L, 3, 0, 0);

    printf("\n");

    lua_close(L);
    return 0;
}