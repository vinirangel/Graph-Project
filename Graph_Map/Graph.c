#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#define NUM_PROCESSES  4

static const char *processText[] = {
    "Menor Caminho",
    "Menor Tarifa",
    "Volta ao mundo",
    "Altitudes"
};

int coordXToPixel(int x){
    return 1280 * (x + 180) /360;
}

int coordYToPixel(int y){
    return 720 * (-(y) + 90) /180;
}

struct Flights {
    int startLineX;
    int startLineY;
    int endLineX;
    int endLineY;
    int distance;
    int price;
    int startIndex;
    int endIndex;
    bool startVertex;
    bool endVertex;
};

typedef struct Path {
    int pathCounter;
    int pathArray[];
};


void printPath(int parent[], int j, struct Path* path)
{
    if (parent[j] == - 1)
        return;

    printPath(parent, parent[j], path);
    path->pathArray[path->pathCounter] = j;
    path->pathCounter++;
}


struct Path* printSolution(int dist[], int graphSize, int parent[], int startVertex, int endVertex, struct Path* path)
{
    path->pathCounter = 1;
    path->pathArray[0] = startVertex;
    printPath(parent, endVertex, path);
    for(int i = 0; i < graphSize; i++)
        printf("\nFINAL PATH = %d[%d]\n",path->pathArray[i],i);
        
    return path;
}

int minDistance(int distance[], bool added[], int graphSize)
{
    int min = INT_MAX;
    int min_index;
    for(int i = 0; i < graphSize; i++)
    {
        if(added[i] == false && distance[i] <= min)
        {
            min = distance[i];
            min_index = i;
        }
    }
    return min_index;
}

struct Path* Dijkstra(struct Flights flights[], int graphSize, int graph[][graphSize], int startVertex, int endVertex, struct Path* path)
{
    int distance[graphSize];
    bool added[graphSize];
    int parents[graphSize];
    parents[startVertex] = -1;
    
    //inicializa todas as distancias com um valor infinito
    for(int i = 0; i < graphSize; i++)
    {
        distance[i] = INT_MAX;
        added[i] = false;
    }
    distance[startVertex] = 0;
    
    //encontra o menor caminho entre os vertices
    for(int count = 0; count < graphSize - 1; count++)
    {
        int minIndex = minDistance(distance, added, graphSize);
        
        //marca o vertice como visitado
        added[minIndex] = true;
        for(int v = 0; v < graphSize; v++)
        {
            //atualiza o vetor de distancias se ele nao foi visitado e se o peso total do caminho for menor do que o valor atual
            if(!added[v] && graph[minIndex][v] && distance[minIndex] + graph[minIndex][v] < distance[v])
            {
                parents[v] = minIndex;
                distance[v] = distance[minIndex] + graph[minIndex][v];
            }
        }
    }
        path =  printSolution(distance, graphSize, parents, startVertex, endVertex, &path);
        return path;
}

int main() {
    InitWindow(1280, 720, "Trabalho Prático - Grafos");
    Image worldMap = LoadImage("worldMap.png");
    Texture2D texture = LoadTextureFromImage(worldMap);    // Image converted to texture, GPU memory (VRAM)
    Rectangle toggleRecs[NUM_PROCESSES] = { 0 };
    int currentProcess = 0;
    int mouseHoverRec = -1;
    int airportQty = 0;
    char tmp[20];
    FILE* fp = fopen("airports.txt", "r");
    if(fp == NULL)
        printf("ARQUIVO NULO");
    fgets(tmp, 10, fp); //ler a quantidade de aeroportos a serem adicionados
    airportQty = atoi(tmp);
    char* sigla[airportQty];
    char* latitude[airportQty];
    char* longitude[airportQty];
    char airports[airportQty][30];
    FILE* test = fopen("TESTE.txt", "w+");
    int i = 0;
    while(fgets(airports[i],sizeof(airports),fp)){
        fprintf(test, "%s", airports[i]);
        i++;
    }
    
    i = 0;
    char* token;
    while (i < airportQty)
    {
        token = strdup(airports[i]);
        token = strtok(token, " ");
        sigla[i] = token;
        token = strtok(NULL, " ");
        latitude[i] = token;
        token = strtok(NULL, "\n");
        longitude[i] = token;
        i++;
    }

    int flightQty = 0;
    char tmp2[20];
    FILE* fp2 = fopen("flights.txt", "r");
    if(fp2 == NULL)
        printf("ARQUIVO NULO");
    fgets(tmp2, 10, fp2); //ler a quantidade de voos a serem adicionados
    FILE* test2 = fopen("TESTE2.txt", "w+");
    flightQty = atoi(tmp2);
    char* flightStart[flightQty];
    char* flightEnd[flightQty];
    char* prices[flightQty];
    char flightString[flightQty][50];
    
    i = 0;
    while(fgets(flightString[i],sizeof(flightString),fp2)){
        fprintf(test2, "%s", flightString[i]);
        i++;
    }  
    
    i = 0;
    char* token2;
    while (i < flightQty){
        token2 = strdup(flightString[i]);
        flightStart[i] = strtok(token2, " ");
        flightEnd[i] = strtok(NULL, " ");
        prices[i] = strtok(NULL, "\n");
        i++;
    }
    
    struct Flights flights[flightQty];
    
    //Converte as latitudes e longitudes em coordenadas do mapa
    for(int j = 0; j < flightQty; j++){
        i = 0;
        flights[j].price = atoi(prices[j]);
        while(i < airportQty){
            fprintf(test,"\nAirport[%i] = %s LAT %s  LONG %s",i,sigla[i],latitude[i],longitude[i]);
            if(strcmp(sigla[i],flightStart[j]) == 0 ){
                flights[j].startLineY = coordYToPixel(atoi(latitude[i]));
                flights[j].startLineX = coordXToPixel(atoi(longitude[i]));
                flights[j].startIndex = i;
                fprintf(test,"\nSTART[%i] = LAT %i  LONG %i",j, flights[j].startLineY, flights[j].startLineX);
            }
            if(strcmp(sigla[i],flightEnd[j]) == 0 ){
                flights[j].endLineY = coordYToPixel(atoi(latitude[i]));
                flights[j].endLineX = coordXToPixel(atoi(longitude[i]));
                flights[j].endIndex = i;
                fprintf(test,"\nEND[%i] = LAT %i  LONG %i",j, flights[j].endLineY, flights[j].endLineX);
            }
            i++;
        }
    }
    
    int selectedVertexes = 0;
    int startVertexIndex = -1;
    int endVertexIndex = -1;
    
    int priceGraph[flightQty][flightQty];
    int distanceGraph[flightQty][flightQty];

    //Inicializa as distancias
    for(int i = 0; i < flightQty; i++)
    {
        for(int j = 0; j < flightQty; j++)
        {
           distanceGraph[i][j] = 0; 
        }
    }

    for(int i = 0; i < flightQty; i++)
    {
        priceGraph[flights[i].startIndex][flights[i].endIndex] = flights[i].price;
        priceGraph[flights[i].endIndex][flights[i].startIndex] = flights[i].price;
        
        //Calcula as distancias atraves do Teorema de Pitagoras
        int distanceX = flights[i].endLineX - flights[i].startIndex;
        int distanceY = flights[i].endLineY - flights[i].startLineY;
        flights[i].distance = sqrt(distanceX*distanceX+distanceY*distanceY);
        
        distanceGraph[flights[i].startIndex][flights[i].endIndex] = flights[i].distance;
        distanceGraph[flights[i].endIndex][flights[i].startIndex] = flights[i].distance;
    }
    
    struct Path* path = (struct Path *)malloc(sizeof(struct Path) + flightQty * sizeof(int));
   
    for (int i = 0; i < NUM_PROCESSES; i++) 
        toggleRecs[i] = (Rectangle){ 15.0f, (float)(500 + 32*i), 300.0f, 30.0f };
    
    SetTargetFPS(60);
    
    while(!WindowShouldClose()){
        
        //Altera os botoes do menu ao serem selecionados
        for (int i = 0; i < NUM_PROCESSES; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), toggleRecs[i]))
            {
                mouseHoverRec = i;

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    currentProcess = i;
                }
                break;
            }
            else mouseHoverRec = -1;
        }
        
       BeginDrawing();
       DrawTexture(texture, 0, 0, RAYWHITE);
       
       //Desenha os menus
       for (int i = 0; i < NUM_PROCESSES; i++)
        {
            DrawRectangleRec(toggleRecs[i], ((i == currentProcess) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
            DrawRectangleLines((int)toggleRecs[i].x, (int) toggleRecs[i].y, (int) toggleRecs[i].width, (int) toggleRecs[i].height, ((i == currentProcess) || (i == mouseHoverRec)) ? BLUE : GRAY);
            DrawText( processText[i], (int)( toggleRecs[i].x + toggleRecs[i].width/2 - MeasureText(processText[i], 10)/2), (int) toggleRecs[i].y + 11, 10, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
        }
        
        //Desenha os aeroportos
        for(int i = 0; i < airportQty; i++)
        {            
            int coordinateX = coordXToPixel(atoi(longitude[i]));
            int coordinateY = coordYToPixel(atoi(latitude[i]));
            if((coordinateX == flights[startVertexIndex].startLineX && flights[startVertexIndex].startVertex) || (coordinateY == flights[endVertexIndex].startLineY && flights[endVertexIndex].endVertex) ){
                DrawCircle(coordinateX, coordinateY, 5, BLACK);
            }
            else{
                DrawCircle(coordinateX, coordinateY, 5, RED);
            }
            DrawText(sigla[i],coordinateX+18,coordinateY-10, 18, RAYWHITE);
        }  
        
        for(int i = 0; i < flightQty; i++){
            DrawLine(flights[i].startLineX,flights[i].startLineY, flights[i].endLineX, flights[i].endLineY, RED);
            char tmp[20];
            //Transforma a variável preco em char
            sprintf(tmp, "%d", flights[i].price);
            //Desenha as rotas
            int tmpX = (flights[i].startLineX + flights[i].endLineX)/2;
            int tmpY = (flights[i].startLineY + flights[i].endLineY)/2;
            DrawText(tmp,tmpX, tmpY-10, 20, BLACK);
        }
        
        //Seleciona o Vertice Inicial dentre os aeroportos
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {   
            int mousePosX = GetMouseX();
            int mousePosY = GetMouseY();
            
            for(int i = 0; i < flightQty; i++)
            {
                //Distância do mouse até o ponto
                if(mousePosX - flights[i].startLineX <= 5 && mousePosY - flights[i].startLineY <= 5 && mousePosX - flights[i].startLineX >= -5 && mousePosY - flights[i].startLineY >= -5 ) 
                {
                    if(selectedVertexes == 0)
                    {
                        selectedVertexes++;
                        flights[i].startVertex = true;
                        startVertexIndex = i;
                        break;
                    }
                    else
                    {
                        flights[i].startVertex = true;
                        startVertexIndex = i;
                        break;
                    }
                }
            }
        }
        
        //Seleciona o Vertice Final dentre os aeroportos
        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {   
            int mousePosX = GetMouseX();
            int mousePosY = GetMouseY();
            
            for(int i = 0; i < flightQty; i++)
            {
                //Distância do mouse até o ponto
                if(mousePosX - flights[i].startLineX <= 5 && mousePosY - flights[i].startLineY <= 5 && mousePosX - flights[i].startLineX >= -5 && mousePosY - flights[i].startLineY >= -5 )
                {
                    if(selectedVertexes == 0)
                    {
                        selectedVertexes++;
                        flights[i].endVertex = true;
                        endVertexIndex = i;
                        break;
                    }
                    else
                    {
                        flights[i].endVertex = true;
                        endVertexIndex = i;
                        break;
                    }
                }
            }
        }    
        
        //Utiliza Dijkstra para calcular a distancia entre as rotas
        if(startVertexIndex != -1 && endVertexIndex != -1 && currentProcess == 0)
        {
            float clockStart = clock();
            path = Dijkstra(flights, flightQty, distanceGraph, startVertexIndex,endVertexIndex, &path);
            float clockEnd = clock();
            //printf("\nTempo decorrido = %f\n", ((double)(clockEnd-clockStart))/CLOCKS_PER_SEC);
            for(int i = 0; i < flightQty; i++)
            {
                if(path->pathArray[i] == endVertexIndex)
                    break;
                for(int j = 0; j < flightQty; j++)
                {
                    if(flights[j].startIndex == path->pathArray[i] && flights[j].endIndex == path->pathArray[i+1] 
                    || (flights[j].startIndex == path->pathArray[i+1] && flights[j].endIndex == path->pathArray[i]))
                    {
                        DrawLine(flights[j].startLineX,flights[j].startLineY, flights[j].endLineX, flights[j].endLineY, BLACK);
                        j = flightQty;
                    }
                }
            }
        }
        
        //Utiliza Dijkstra para calcular o custo entre as rotas
        if(startVertexIndex != -1 && endVertexIndex != -1 && currentProcess == 1)
        {
            path = Dijkstra(flights, flightQty, priceGraph, startVertexIndex,endVertexIndex, &path);
            for(int i = 0; i < flightQty; i++)
            {
                if(path->pathArray[i] == endVertexIndex)
                    break;
                for(int j = 0; j < flightQty; j++)
                {
                    if(flights[j].startIndex == path->pathArray[i] && flights[j].endIndex == path->pathArray[i+1] 
                    || (flights[j].startIndex == path->pathArray[i+1] && flights[j].endIndex == path->pathArray[i]))
                    {
                        DrawLine(flights[j].startLineX,flights[j].startLineY, flights[j].endLineX, flights[j].endLineY, BLACK);
                        j = flightQty;
                        }
                }
            }
        }
        
       free(path);
       ClearBackground(RAYWHITE);
       EndDrawing();
    }
    CloseWindow();  
    
    return 0;
}