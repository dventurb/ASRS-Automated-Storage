#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

#ifndef MAX
#define MAX 9
#endif

// STRUCTS 
typedef struct {
  char nome[100];
  unsigned char ID[4];
}ST_FUNCIONARIO;

typedef struct {
  bool estado;
  unsigned int peso;
  ST_FUNCIONARIO *funcionario;
} ST_COMPARTIMENTO;

extern ST_COMPARTIMENTO compartimentos[MAX];

// HTML
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ASRS Compartimentos</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; }
        .container {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            gap: 20px;
            margin-top: 20px;
        }
        .compartimento {
            width: 70px;
            height: 70px;
            border-radius: 5px;
            display: flex;
            justify-content: center;
            align-items: center;
            font-weight: bold;
        }
        .livre { background-color: green; }
        .ocupado { background-color: red; }
        .peso {
            margin-top: 5px;
            font-size: 14px;
        }
        .compartimento-container {
            text-align: center;
        }
    </style>
    <script>
        function atualizarCompartimentos() {
            fetch('/status')
            .then(response => response.json())
            .then(data => {
                for (let i = 0; i < 9; i++) {
                    let compartimento = document.getElementById("comp" + i);
                    let peso = document.getElementById("peso" + i);
                    compartimento.className = data[i].estado ? "compartimento ocupado" : "compartimento livre";
                    if (data[i].funcionario) {
                        compartimento.innerText = `${data[i].funcionario.nome}`;
                    } else {
                        compartimento.innerText = "Vazio";
                    }
                    peso.innerText = `${data[i].peso}gr`;
                }
            });
        }
        setInterval(atualizarCompartimentos, 1000);
    </script>
</head>
<body>
    <h1>Estado dos Compartimentos</h1>
    <div class="container">
        <div class="compartimento-container">
            <div id="comp8" class="compartimento livre">9</div>
            <div id="peso8" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp7" class="compartimento livre">8</div>
            <div id="peso7" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp6" class="compartimento livre">7</div>
            <div id="peso6" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp5" class="compartimento livre">6</div>
            <div id="peso5" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp4" class="compartimento livre">5</div>
            <div id="peso4" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp3" class="compartimento livre">4</div>
            <div id="peso3" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp2" class="compartimento livre">3</div>
            <div id="peso2" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp1" class="compartimento livre">2</div>
            <div id="peso1" class="peso">0gr</div>
        </div>
        
        <div class="compartimento-container">
            <div id="comp0" class="compartimento livre">1</div>
            <div id="peso0" class="peso">0gr</div>
        </div>
    </div>
</body>
</html>
)rawliteral";

// Retornar o estado dos compartimentos
String estadoJson(void) {
    String json = "[";
    for (int i = 0; i < MAX; i++) {
        json += "{";
        json += "\"estado\":" + String(compartimentos[i].estado ? "1" : "0") + ",";
        if (compartimentos[i].funcionario != nullptr) {
            json += "\"funcionario\": {\"nome\": \"" + String(compartimentos[i].funcionario->nome) + "\"},";
        }
        json += "\"peso\":" + String(compartimentos[i].peso);
        json += "}";
        if (i < MAX - 1) json += ",";
    }
    json += "]";
    return json;
}

void iniciarWebServer(void) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", htmlPage);
    });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", estadoJson());
    });

    server.begin();
}

#endif
