#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "Include/qdbmp.h"


using namespace std;


int LoadImage(const char* filename) {
    BMP* folha = BMP_ReadFile(filename);
    int  largura, altura, profundidade;
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao abrir a imagem: " << filename << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
        largura = 0;
        altura = 0;
        profundidade = 0;
        return 1;
    }

    altura = BMP_GetHeight(folha);
    largura = BMP_GetWidth(folha);
    profundidade = BMP_GetDepth(folha);



    BMP* folhaBW = BMP_Create(largura, altura, 24);
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao criar a imagem BW!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folha);
    }

    for (int x = 0; x < largura; ++x) {
            for (int y = 0; y < altura; ++y) {
                unsigned char r, g, b;
                unsigned char gray;

                BMP_GetPixelRGB(folha, x, y, &r, &g, &b);

                gray = (unsigned char)((0.299f * r) + (0.587f * g) + (0.114f * b));
                    if (gray < 160)
                        gray = 255;
                    else
                        gray = 0;



                BMP_SetPixelRGB(folhaBW, x, y, gray, gray, gray);
            }
        }
    
        BMP_WriteFile(folhaBW, "BlackWhite/FolhasBW.bmp");
        if (BMP_GetError() != BMP_OK) {
            cout << "Erro ao salvar a imagem!" << endl;
            cout << "Codigo do erro: " << BMP_GetError() << endl;
            cout << "Descricao: " << BMP_GetErrorDescription() << endl;
        }
        BMP_Free(folha);
        BMP_Free(folhaBW);

        return 0;
}

void Dilatacao(const char* filename) {
    BMP* folhaBW = BMP_ReadFile(filename);
    int  largura, altura, profundidade;
    altura = BMP_GetHeight(folhaBW);
    largura = BMP_GetWidth(folhaBW);
    profundidade = BMP_GetDepth(folhaBW);
    int kernel[3][3] = {
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0}
    };
    
    BMP* folhaDilatada = BMP_Create(largura, altura, 24);
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao criar a imagem dilatada!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folhaBW);

    }

    for (int y = 0; y < altura; ++y) {
            for (int x = 0; x < largura; ++x) {
                unsigned char r, g, b;
                BMP_GetPixelRGB(folhaBW, x, y, &r, &g, &b);
                if (r == 255) { // Pixel branco
                    for (int ky = -1; ky <= 1; ++ky) {
                        for (int kx = -1; kx <= 1; ++kx) {
                            if (kernel[ky + 1][kx + 1] == 1) {
                                int newX = x + kx;
                                int newY = y + ky;
                                if (newX >= 0 && newX < largura && newY >= 0 && newY < altura) {
                                    BMP_SetPixelRGB(folhaDilatada, newX, newY, 255, 255, 255);
                                }
                            }
                        }
                    }
                }
            }
        }

    BMP_WriteFile(folhaDilatada, "Dilatacao/FolhasDilatadas.bmp");
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao salvar a imagem dilatada!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
    }
    BMP_Free(folhaDilatada);
    BMP_Free(folhaBW);

}

void Erosao(const char* filename) {
    BMP* folhaBW = BMP_ReadFile(filename);
    int  largura, altura, profundidade;
    altura = BMP_GetHeight(folhaBW);
    largura = BMP_GetWidth(folhaBW);
    profundidade = BMP_GetDepth(folhaBW);
    int kernel[3][3] = {
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0}
    };

    BMP* folhasErosao = BMP_Create(largura, altura, 24);
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao criar a imagem erodida!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folhaBW);
    }

    for (int y = 0; y < altura; ++y) {
            for (int x = 0; x < largura; ++x) {
                unsigned char r, g, b;
                BMP_GetPixelRGB(folhaBW, x, y, &r, &g, &b);
                if (r == 255) { // Pixel branco
                    bool erodir = false;
                    for (int ky = -1; ky <= 1; ++ky) {
                        for (int kx = -1; kx <= 1; ++kx) {
                            if (kernel[ky + 1][kx + 1] == 1) {
                                int newX = x + kx;
                                int newY = y + ky;
                                if (newX >= 0 && newX < largura && newY >= 0 && newY < altura) {
                                    unsigned char nr, ng, nb;
                                    BMP_GetPixelRGB(folhaBW, newX, newY, &nr, &ng, &nb);
                                    if (nr == 0) { // Pixel preto
                                        erodir = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if (erodir) break;
                    }
                    if (!erodir) {
                        BMP_SetPixelRGB(folhasErosao, x, y, 255, 255, 255);
                    }
                }
            }
        }

    BMP_WriteFile(folhasErosao, "Erosao/FolhasErosao.bmp");
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao salvar a imagem erodida!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
    }
    BMP_Free(folhasErosao);
    BMP_Free(folhaBW);

}

void Bordas(const char* Imagem1, const char* Imagem2){
    BMP* folha1 = BMP_ReadFile(Imagem1);
    BMP* folha2 = BMP_ReadFile(Imagem2);

    int largura = BMP_GetWidth(folha1);
    int altura = BMP_GetHeight(folha1);

    BMP* folhaBordas = BMP_Create(largura, altura, 24);
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao criar a imagem de bordas!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folha1);
        BMP_Free(folha2);
    }

    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            unsigned char r1, g1, b1;
            unsigned char r2, g2, b2;

            BMP_GetPixelRGB(folha1, x, y, &r1, &g1, &b1);
            BMP_GetPixelRGB(folha2, x, y, &r2, &g2, &b2);

            if (r1 == 255 && r2 == 0) { // Pixel branco na primeira imagem e preto na segunda
                BMP_SetPixelRGB(folhaBordas, x, y, 255, 0, 0); // Definir pixel como vermelho
            } else {
                BMP_SetPixelRGB(folhaBordas, x, y, 0, 0, 0); // Definir pixel como preto
            }
            
        }
    }

    BMP_WriteFile(folhaBordas, "Bordas/FolhasBordas.bmp");
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao salvar a imagem de bordas!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
    }
    
    BMP_Free(folha1);
    BMP_Free(folha2);
    BMP_Free(folhaBordas);

}

void DesenhaOriginal(const char* Imagem1, const char* Imagem2) {
    BMP* folha1 = BMP_ReadFile(Imagem1);
    BMP* folha2 = BMP_ReadFile(Imagem2);

    int largura = BMP_GetWidth(folha1);
    int altura = BMP_GetHeight(folha1);

    BMP* folhaDesenhada = BMP_Create(largura, altura, 24);
    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao criar a imagem desenhada!" << endl;
        cout << "Codigo do erro: " << BMP_GetError() << endl;
        cout << "Descricao: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folha1);
        BMP_Free(folha2);
    }


}

void SegueFronteira(const char* Imagem1) {
    BMP* folha1 = BMP_ReadFile(Imagem1);
    int  largura, altura, profundidade;
    altura = BMP_GetHeight(folha1);
    largura = BMP_GetWidth(folha1);

    bool Find = false;
    int b0[] = {0, 0};
    int c0[] = {0, 0};
    
    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {

            unsigned char r, g, b;
            BMP_GetPixelRGB(folha1, x, y, &r, &g, &b);

            if(r == 255 && g == 0 && b == 0) { // Pixel Vermelho
                b0[0] = x;
                b0[1] = y;
                c0[0] = x-1;
                c0[1] = y;
                Find = true;
                break;
            } else {
                continue;
            }
            if (Find == true) {
                break;
            }
        }
    }

    int dx[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
    int dy[8] = {-1, -1,  0,  1,  1,  1,  0, -1};

    for (int i = 0; i < 8; ++i) {
        int newX = c0[0] + dx[i];
        int newY = c0[1] + dy[i];

        
    }
    //Não consegui implementar a função de seguir a fronteira, mas consegui encontrar o ponto inicial da fronteira. 
}

void rotula_folhas(){
    //Avaliar a imagem binaria em preto e branco
    //cada pixel branco da primeira folha vira 1, da segunda vira 2
    //pixels pretos permanecem 0

    BMP* folhaBW = BMP_ReadFile("BlackWhite/FolhasBW.bmp");   // abre a imagem
    int largura = BMP_GetWidth(folhaBW);                       // quantas colunas
    int altura  = BMP_GetHeight(folhaBW);                      // quantas linhas
    int folhas = 0;                                                  // contador de folhas

    unsigned char r, g, b;
    vector<int> rotulos(largura * altura, 0);
    int contador = 0;
    stack<pair<int, int>> pilha;


    for(int y = 0; y < altura; ++y) {
        for(int x = 0; x < largura; ++x) {



            BMP_GetPixelRGB(folhaBW, x, y, &r, &g, &b);
            if(r == 255 && g == 255 && b == 255 && rotulos[y * largura + x] == 0) { // pixel branco
                contador++;
                rotulos[y * largura + x] = contador;
                pilha.push({x, y});
                //enquanto a pilha nao estiver vazia, pega o pixel do topo da pilha e verifica os vizinhos
                while(!pilha.empty()) {
                    pair<int, int> pixel = pilha.top();
                    pilha.pop();
                    //PERCORRER OS 8 VIZINHO

                    for(int dy = -1; dy <= 1; ++dy) {       
                        for(int dx = -1; dx <= 1; ++dx) {
                            if(dx == 0 && dy == 0) continue; // ignora o pixel atual
                            int nx = pixel.first + dx;
                            int ny = pixel.second + dy;
                            //VE SE O VIZINHO FAZ PARTE DA IMAGE

                            if(nx >= 0 && nx < largura && ny >= 0 && ny < altura) {
                                BMP_GetPixelRGB(folhaBW, nx, ny, &r, &g, &b);
                                if(r == 255 && g == 255 && b == 255 && rotulos[ny * largura + nx] == 0) {
                                    rotulos[ny * largura + nx] = contador;
                                    pilha.push({nx, ny});
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //medir cada rotulo 
    vector<int> areas(contador + 1, 0);
    for(int i = 0; i < rotulos.size(); ++i) {
        if(rotulos[i] > 0)
            areas[rotulos[i]]++;
    }


    //PEGAR O NUMERO DE FOLHAS E IMPRIMIR 

    for(int i = 0; i < areas.size(); ++i) {
        int npixels = 0;
        
        if(areas[i] > 0)
            npixels = areas[i];

        if(npixels > 1000)
            folhas++;
    }

    cout << "Numero de folhas: " << folhas << endl;
    BMP_Free(folhaBW);
}

int main() {
    LoadImage("Folhas/Folhas6.bmp");
    rotula_folhas();
    Dilatacao("BlackWhite/FolhasBW.bmp");
    Erosao("BlackWhite/FolhasBW.bmp");
    Bordas("BlackWhite/FolhasBW.bmp", "Erosao/FolhasErosao.bmp");
    //Bordas("../Dilatacao/FolhasDilatadas.bmp", "../Erosao/FolhasErosao.bmp");
    cout << "Processamento concluído!" << endl;

    return 0;
}
