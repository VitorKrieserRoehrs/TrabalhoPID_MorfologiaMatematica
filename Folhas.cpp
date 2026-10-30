#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "Include/qdbmp.h"
#include <algorithm>
#include <filesystem>
#include <fstream>

using namespace std;

int LoadImage(const char* filename) {
    BMP* folha = BMP_ReadFile(filename);
    int largura, altura, profundidade;

    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao abrir a imagem: " << filename << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
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
        cout << "Erro ao criar a imagem BW." << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folha);
        return 1;
    }

    for (int x = 0; x < largura; ++x) {
        for (int y = 0; y < altura; ++y) {
            unsigned char r, g, b;
            unsigned char gray;

            BMP_GetPixelRGB(folha, x, y, &r, &g, &b);

            gray = (unsigned char)((0.299f * r) + (0.587f * g) + (0.114f * b));
            // eu atualizei essa parte logo à frente porque, em algumas imagens, o algoritmo estava detectando a sombra como parte da folha
            // em resumo, a sombra tende a ser cinza (pouca cor), mas a folha tem uma cor (bem) mais viva, então foi só atualizar os parâmetros dentro do if
            // e fazer a análise do cinza e da variável "cor" abaixo
            int cor = max(r, max(g, b)) - min(r, min(g, b)); // diferença entre maior e menor canal RGB

            if (gray < 130 || (gray < 170 && cor > 30))
                gray = 255;
            else
                gray = 0;
            
            // o problema ainda PODE persistir nas folhas com uma coloração bem escura, mas ainda foi uma melhoria, principalmente no caso do Coleus, que é claro e bem colorido

            BMP_SetPixelRGB(folhaBW, x, y, gray, gray, gray);
        }
    }

    BMP_WriteFile(folhaBW, "BlackWhite/FolhasBW.bmp");

    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao salvar a imagem." << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
    }

    BMP_Free(folha);
    BMP_Free(folhaBW);
    return 0;
}

void Dilatacao(const char* filename) {
    BMP* folhaBW = BMP_ReadFile(filename);
    int largura, altura, profundidade;

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
        cout << "Erro ao criar a imagem dilatada." << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folhaBW);
        return;
    }

    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            unsigned char r, g, b;
            BMP_GetPixelRGB(folhaBW, x, y, &r, &g, &b);

            if (r == 255) { // se piixel branco
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
        cout << "Erro ao salvar a imagem dilatada." << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
    }

    BMP_Free(folhaDilatada);
    BMP_Free(folhaBW);
}

void Erosao(const char* filename) {
    BMP* folhaBW = BMP_ReadFile(filename);
    int largura, altura, profundidade;

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
        cout << "Erro ao criar a imagem erodida." << endl; // erodida?
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folhaBW);
        return;
    }

    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            unsigned char r, g, b;
            BMP_GetPixelRGB(folhaBW, x, y, &r, &g, &b);

            if (r == 255) { // se pixel branco
                bool erodir = false;

                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        if (kernel[ky + 1][kx + 1] == 1) {
                            int newX = x + kx;
                            int newY = y + ky;

                            if (newX >= 0 && newX < largura && newY >= 0 && newY < altura) {
                                unsigned char nr, ng, nb;
                                BMP_GetPixelRGB(folhaBW, newX, newY, &nr, &ng, &nb);

                                if (nr == 0) { // se pixel preto
                                    erodir = true;
                                    break;
                                }
                            } else {
                                // fora da imagem conta como fundo
                                erodir = true;
                                break;
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
        cout << "Erro ao salvar a imagem erodida." << endl;
        cout << "ódigo de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
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
        cout << "Erro ao criar a imagem de bordas." << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
        BMP_Free(folha1);
        BMP_Free(folha2);
        return;
    }

    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            unsigned char r1, g1, b1;
            unsigned char r2, g2, b2;

            BMP_GetPixelRGB(folha1, x, y, &r1, &g1, &b1);
            BMP_GetPixelRGB(folha2, x, y, &r2, &g2, &b2);

            if (r1 == 255 && r2 == 0) { // pixel branco na primeira imagem e preto na segunda
                BMP_SetPixelRGB(folhaBordas, x, y, 255, 0, 0); // definir pixel como vermelho
            } else {
                BMP_SetPixelRGB(folhaBordas, x, y, 0, 0, 0); // definir pixel como preto
            }
        }
    }

    BMP_WriteFile(folhaBordas, "Bordas/FolhasBordas.bmp");

    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao salvar a imagem de bordas." << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
    }

    BMP_Free(folha1);
    BMP_Free(folha2);
    BMP_Free(folhaBordas);
}

void BordasPorFolha(vector<int>& rotulos, vector<int>& areas, int largura, int altura, vector<int>& bordas) {
    // pega a borda feita pela erosão e separa por folha
    BMP* folhaBordas = BMP_ReadFile("Bordas/FolhasBordas.bmp");

    bordas.assign(largura * altura, 0);

    for(int y = 0; y < altura; ++y) {
        for(int x = 0; x < largura; ++x) {
            unsigned char r, g, b;
            BMP_GetPixelRGB(folhaBordas, x, y, &r, &g, &b);

            int rotulo = rotulos[y * largura + x];

            // pixel vermelho de uma folha válida
            if(r == 255 && g == 0 && rotulo > 0 && areas[rotulo] > 1000) {
                bordas[y * largura + x] = rotulo;
            }
        }
    }

    BMP_Free(folhaBordas);
}

void DesenhaOriginal(BMP* folha, vector<int>& fronteira, int rotulo, int largura, int altura, int numero) {
    // pinta de vermelho a fronteira da folha na imagem original
    for(int y = 0; y < altura; ++y) {
        for(int x = 0; x < largura; ++x) {
            if(fronteira[y * largura + x] == rotulo) {
                BMP_SetPixelRGB(folha, x, y, 255, 0, 0);
            }
        }
    }

    string nome = "output/Folha" + to_string(numero) + ".bmp";
    BMP_WriteFile(folha, nome.c_str());

    if (BMP_GetError() != BMP_OK) {
        cout << "Erro ao salvar a imagem desenhada." << endl;
        cout << "Código de erro: " << BMP_GetError() << endl;
        cout << "Descrição: " << BMP_GetErrorDescription() << endl;
    }
}

int rotula_folhas(vector<int>& rotulos, vector<int>& areas, int& largura, int& altura){
    //avaliar a imagem binária em preto e branco
    //cada pixel branco da primeira folha vira 1, da segunda vira 2
    //pixels pretos permanecem 0
    BMP* folhaBW = BMP_ReadFile("BlackWhite/FolhasBW.bmp"); // abre a imagem

    largura = BMP_GetWidth(folhaBW); // quantas colunas
    altura = BMP_GetHeight(folhaBW); // quantas linhas

    int folhas = 0; // contador de folhas
    unsigned char r, g, b;
    rotulos.assign(largura * altura, 0);
    int contador = 0;
    stack<pair<int, int>> pilha;

    for(int y = 0; y < altura; ++y) {
        for(int x = 0; x < largura; ++x) {
            BMP_GetPixelRGB(folhaBW, x, y, &r, &g, &b);

            if(r == 255 && g == 255 && b == 255 && rotulos[y * largura + x] == 0) { // pixel branco
                contador++;
                rotulos[y * largura + x] = contador;
                pilha.push({x, y});

                // enquanto a pilha não estiver vazia, pega o pixel do topo da pilha e verifica os vizinhos
                while(!pilha.empty()) {
                    pair<int, int> pixel = pilha.top();
                    pilha.pop();

                    // PERCORRER OS 8 VIZINHO
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

    BMP_Free(folhaBW);

    //medir cada rótulo
    areas.assign(contador + 1, 0);
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

    return folhas;
}

int seguidor_fronteira(vector<int>& bordas, vector<int>& fronteira, int rotulo, int largura, int altura) {
    // implementação algoritmo de seguimento de fronteira (Moore, Gonzales págs 814-816. testar se ele não tá processando o ponto inicial (?) 2x)
    // 8 vizinhos em sentido horário, começando pelo da esquerda (também descrito como ao oeste)
    int dx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int dy[8] = {0, -1, -1, -1, 0, 1, 1, 1};

    // acha o primeiro pixel da borda da folha
    int x0 = -1, y0 = -1;

    for(int y = 0; y < altura && x0 == -1; ++y) {
        for(int x = 0; x < largura; ++x) {
            if(bordas[y * largura + x] == rotulo) {
                x0 = x;
                y0 = y;
                break;
            }
        }
    }

    if(x0 == -1){
        return 0;
    } 

    int x = x0, y = y0;
    int cx = x0 - 1, cy = y0; // pixel de fundo de onde chegou
    int x1 = -1, y1 = -1; // segundo pixel da fronteira
    int contador = 1;

    fronteira[y0 * largura + x0] = rotulo;

    while(true) {
        // vendo em qual direção está o pixel de fundo
        int dir = 0;
        for(int i = 0; i < 8; ++i) {
            if(x + dx[i] == cx && y + dy[i] == cy) dir = i;
        }

        // gira em volta do pixel atual até achar o próximo da borda
        bool achou = false;
        int nx = 0, ny = 0, k = 0;

        for(int i = 1; i <= 8; ++i) {
            k = (dir + i) % 8;
            nx = x + dx[k];
            ny = y + dy[k];

            if(nx >= 0 && nx < largura && ny >= 0 && ny < altura && bordas[ny * largura + nx] == rotulo) {
                achou = true;
                break;
            }
        }

        if(!achou) break;

        // o pixel de fundo novo e o vizinho visto antes do que foi achado
        int anterior = (k + 7) % 8;
        cx = x + dx[anterior];
        cy = y + dy[anterior];

        // para quando voltar no início e o próximo for o segundo pixel
        if(x1 == -1) {
            x1 = nx;
            y1 = ny;
        } else if(x == x0 && y == y0 && nx == x1 && ny == y1) {
            break;
        }

        x = nx;
        y = ny;

        if(fronteira[y * largura + x] == 0) {
            fronteira[y * largura + x] = rotulo;
            contador++;
        }
    }

    return contador;
}


int main(int argc, char* argv[]) {
    bool modoLote = (argc > 1);

    vector<string> arquivos;
    if (modoLote) {
        for (const auto& entrada : filesystem::directory_iterator("Folhas")) {
            string nome = entrada.path().filename().string();
            if (entrada.path().extension() == ".bmp" && nome.rfind("IMG_", 0) == 0)
                arquivos.push_back(nome);
        }
        sort(arquivos.begin(), arquivos.end()); // ordem estável para o relatório
    } else {
        string nomeArquivo;
        cout << "Digite o nome da imagem (ex: Folhas10.bmp): ";
        getline(cin, nomeArquivo);
        size_t inicio = nomeArquivo.find_first_not_of(" \t\r\n");
        size_t fim = nomeArquivo.find_last_not_of(" \t\r\n");
        if (inicio == string::npos) {
            cout << "Nenhum nome foi informado. Encerrando o programa." << endl;
            return 1;
        }
        arquivos.push_back(nomeArquivo.substr(inicio, fim - inicio + 1));
    }

    ofstream csv;
    if (modoLote) {
        csv.open("output/relatorio.csv");
        if (!csv) {
            cout << "Erro ao abrir output/relatorio.csv. Verifique se a pasta output/ existe." << endl;
            return 1;
        }
        csv << "imagem,folha,area,pixels_fronteira\n";
    }

    for (const string& nomeArquivo : arquivos) {
        string caminho = "Folhas/" + nomeArquivo;
        cout << "\n=== " << caminho << " ===" << endl;

        if (LoadImage(caminho.c_str()) != 0) {
            cout << "Imagem nao encontrada ou invalida: " << caminho << endl;
            continue;
        }

        string nomeBase = nomeArquivo;
        size_t ponto = nomeBase.find_last_of('.');
        if (ponto != string::npos) nomeBase = nomeBase.substr(0, ponto);

        vector<int> rotulos;
        vector<int> areas;
        vector<int> bordas;
        int largura = 0, altura = 0;

        int folhas = rotula_folhas(rotulos, areas, largura, altura);
        cout << "Numero de folhas: " << folhas << endl;

        Dilatacao("BlackWhite/FolhasBW.bmp");
        Erosao("BlackWhite/FolhasBW.bmp");
        Bordas("BlackWhite/FolhasBW.bmp", "Erosao/FolhasErosao.bmp");
        BordasPorFolha(rotulos, areas, largura, altura, bordas);

        vector<int> fronteira(largura * altura, 0);
        BMP* resultado = BMP_ReadFile(caminho.c_str());
        int numero = 0;

        for(int rotulo = 1; rotulo < (int)areas.size(); ++rotulo) {
            if(areas[rotulo] > 1000) {
                numero++;

                int pixels = seguidor_fronteira(bordas, fronteira, rotulo, largura, altura);
                DesenhaOriginal(resultado, fronteira, rotulo, largura, altura, numero);

                string antigo = "output/Folha" + to_string(numero) + ".bmp";
                string novo   = "output/" + nomeBase + "_folha" + to_string(numero) + ".bmp";
                if (rename(antigo.c_str(), novo.c_str()) != 0)
                    cout << "Aviso: nao foi possivel renomear " << antigo << " para " << novo << endl;

                csv << nomeBase << "," << numero << "," << areas[rotulo] << "," << pixels << "\n";

                cout << "Folha " << numero << ": " << pixels << " pixels de borda" << endl;
                cout << "Imagem salva em " << novo << endl;

                if (!modoLote) {
                    cout << "Aperte Enter para ir para a proxima folha." << endl;
                    cin.get();
                }
            }
        }

        BMP_Free(resultado);
    }

    cout << "\nProcessamento concluido!" << endl;
    return 0;
}