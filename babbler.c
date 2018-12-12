#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// #include <lua.h>
// #include <lualib.h>
// #include <lauxlib.h>

#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

int readFile(lua_State *L) {
    const char *filename = lua_tostring(L, 1);
    char *buffer;
    int rc;
    long length;
    FILE *fin = fopen(filename, "rb");

    if(fin) {
        rc = fseek(fin, 0, SEEK_END);
        assert(rc >= 0);
        length = ftell(fin);
        rc = fseek(fin, 0, SEEK_SET);
        assert(rc >= 0);
        buffer = malloc(length + 1);
        if( buffer ) {
            fread(buffer, length, 1, fin);
        }
        
        rc = fclose(fin);
        assert(rc >= 0);

        // printf(buffer);
        buffer[length] = 0;
        lua_pushstring(L, buffer);
        free(buffer);
    }

    return 1;
}

// You should write a C function that takes the file content string and an offset into it file as parameters. It returns the next available token as a Lua string, and the offset into the string where the search for the next token should begin. If there are no tokens left in the string, it should return the empty string instead of a valid token. The function should use the following rules:

int parseInput(lua_State *L) {
    const char *corpus = lua_tostring(L, 1);
    const int corpuslen = lua_rawlen(L, 1);
    const int offset = lua_tonumber(L, 2);
    char *rawtoken, *token;
    int start, end;
    size_t toklen;

    // splits corpus by spaces
    start = offset;
    while(start < corpuslen && isspace(corpus[start])) {
        start++;
    }

    end = start;
    while(end < corpuslen && !isspace(corpus[end])) {
        end++;
    }

    // create buffer to store raw token and processed token + null term
    toklen = end - start;
    rawtoken = malloc(toklen + 1);
    token = malloc(toklen + 1);

    for(int i = 0; i < toklen; i++) {
        rawtoken[i] = corpus[start + i];
    }

    // process raw token
    int j = 0;
    for(int i = 0; i < toklen; i++) {
        // if is alphanumeric
        if( isalnum(rawtoken[i]) ) {
            // to lowercase
            token[j] = tolower(rawtoken[i]);
            j++;
        }
    }

    // send to lua if it's not a project gutenberg page number

    if((j == 5 || j == 6) && (token[j - 1] == 'm' && isdigit(token[j - 2]) && isdigit(token[j - 3]))) {
        lua_pushstring(L, 0);
        lua_pushinteger(L, end); // new offset
    } else {
        // lua_pushlstring(L, token, toklen);

        // null-terminate (lua_pushstring vs. pushlstring)
        token[j] = 0;
        lua_pushstring(L, token);
        lua_pushinteger(L, end); // new offset
    }

    free(rawtoken);
    free(token);

    return 2;
}

int main(int argc, char *argv[]) {
    // The C program starts out in main
    // It checks and parses the command-line arguments.
    if(argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <words> [n]", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    int wordcount = atoi(argv[2]), n = 3;

    if(argc == 4) {
        n = atoi(argv[3]);
    }

    // It initializes the Lua environment and registers C functions    that can be called from Lua

    // Create new Lua state and load the lua libraries
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "readFile", readFile);
    lua_register(L, "parseInput", parseInput);

    // It calls a Lua function to drive the rest of the process

    luaL_dofile(L, "babbler.lua");
    lua_getglobal(L, "main");
    lua_pushstring(L, filename);
    lua_pushnumber(L, wordcount);
    lua_pushnumber(L, n);
    lua_pcall(L, 3, 0, 0);

    printf("\n");

    lua_close(L);
    return 0;
}