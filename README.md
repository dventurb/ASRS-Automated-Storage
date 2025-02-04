# Automated Storage and Retrieval System (ASRS)

Este repositório cobre o desenvolvimento de um **Sistema de Armazenamento e Recuperação Automatizado (ASRS)** de baixo custo, utilizando componentes eletrónicos como Arduino, ESP32, motores de passo e módulos RFID. O objetivo do projeto é criar um sistema acessível e fácil de implementar, ideal para fins educativos ou para pequenas empresas que desejam automatizar o armazenamento de materiais.

Aqui, encontrarás todos os ficheiros CAD para as peças impressas em 3D, diagramas elétricos, código-fonte para o Arduino e ESP32, e os resultados dos testes realizados. Se tiveres dúvidas ou comentários, não hesites em contactar-me!

O sistema foi desenvolvido como parte do projeto final do Curso Técnico Superior Profissional de Sistemas Eletrónicos e Computadores, sob a orientação dos professores da **Escola Superior de Tecnologia e Gestão (ESTG)**.

<div align="center">
    <img src="https://imgur.com/B0eKkpb.png" alt="script" width="300">
</div>


### Funcionalidades Principais:
1. **Movimentação Automatizada**: Controlo preciso dos eixos X, Y e Z para armazenamento e recuperação de materiais.
2. **Autenticação de Operadores**: Utilização de um módulo RFID para garantir que apenas operadores autorizados possam interagir com o sistema.
3. **Verificação de Peso**: Integração de um módulo de peso para garantir que os materiais não excedam a capacidade máxima do sistema.
4. **Interface de Controlo**: Interface web desenvolvida no ESP32 para controlo e monitorização do sistema em tempo real.

---

## Estrutura do Repositório

- **/CAD**: Ficheiros STL e STEP para as peças impressas em 3D.
- **/Electronics**: Diagramas elétricos e esquemas de ligação dos componentes.
- **/Software**: Código-fonte para o Arduino e ESP32.
- **/Pictures**: Figuras dos testes realizados e análise.
- **/Documentação**: Relatórios e documentos relevantes.

---

## Componentes Utilizados

| **Componente**            | **Quantidade** | **Descrição**                                                                      |
|----------------------------|----------------|-----------------------------------------------------------------------------------|
| Arduino Mega 2560          | 1              | Microcontrolador principal para controlo dos motores de passo.                    |
| ESP32                      | 1              | Responsável pela interação do utilizador, web server e comunicação com o Arduino. |
| Motores de Passo NEMA 17   | 3              | Utilizados para a movimentação dos eixos X, Y e Z.                                |
| Módulo RFID RC522          | 1              | Para autenticação do utilizador.                                                  |
| Módulo de Peso HX711       | 1              | Para medição do peso dos materiais.                                               |
| Eixos Lineares             | 3              | Para movimentação suave e precisa nos eixos X e Z.                                |
| Parafuso de Avanço         | 1              | Para movimentação vertical no eixo Y.                                             |
| Fonte de Alimentação       | 1              | Para alimentação geral do sistema.                                                |

---

## Montagem e Funcionamento

### Estrutura Mecânica
A estrutura do sistema foi construída principalmente em madeira, com suportes impressos em 3D para os motores e eixos. O eixo Y utiliza um parafuso de avanço para movimentação vertical, enquanto os eixos X e Z utilizam correias dentadas para movimentação horizontal.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Mechanics/ASRS_MechanicalComponents.png" alt="script" width="300">
</div>

### Funcionamento
**1. O ESP32 recebe autenticação via RFID e habilita o controlo do sistema.**

**2. O utilizador controla o sistema através dos Botões de Contato.**

**3. O ESP32 processa o algoritmo de FIFO e envia, via serial, o compartimento a ser tratado para o Arduino Mega.**

**4. O Arduino Mega interpreta os comandos e aciona os motores de passo para movimentação nos eixos X, Y e Z.**

**5. Os sensores garantem precisão e segurança nos movimentos.**

**6. O sistema registra a posição, o peso do material e a Tag RFID do utilizador.**

### Diagrama de Blocos

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Functional_Data_Block.png" alt="script" width="1000">
</div>
   
### Circuito Eletrónico
Este é o esquema elétrico detalhado do sistema ASRS, desenvolvido no KiCad. Ele apresenta todas as conexões elétricas, incluindo componentes como ESP32, Arduino Mega, drivers de motor, sensores, fonte de alimentação e demais módulos eletrônicos utilizados no projeto.

[<img src="https://i.imgur.com/aVkR3sI.png" alt="Esquema" width="1000">](https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Schematic_Colours.pdf)

Este diagrama fornece uma representação visual das ligações entre os principais componentes do ASRS. Ele facilita a montagem e a compreensão das conexões entre os microcontroladores, motores, sensores e módulos auxiliares. Ideal para consultas rápidas e montagem prática do circuito.

<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Circuit_Connections.png" alt="script" width="1000">


### Interface de Controlo
A interface de controlo foi desenvolvida utilizando o ESP32, permitindo que os operadores interajam com o sistema através de um browser. A interface permite:
- Autenticação via RFID.
- Controlo dos movimentos dos eixos.
- Verificação do peso dos materiais.
- Registo de operações em tempo real.

---

## Testes e Resultados

Foram realizados vários testes para validar o funcionamento do sistema, incluindo:
1. **Testes de Movimentação**: Verificação da precisão dos movimentos nos eixos X, Y e Z.
2. **Testes de Autenticação**: Validação do módulo RFID para autenticação de operadores.
3. **Testes de Peso**: Verificação da precisão do módulo de peso.

### Resultados dos Testes
| **Teste**                  | **Resultado**                                                                 |
|----------------------------|-------------------------------------------------------------------------------|
| Movimentação dos Eixos     | Precisão de ±1 mm nos eixos X e Z, e ±0.5 mm no eixo Y.                      |
| Autenticação RFID          | 100% de precisão na leitura dos cartões RFID.                                |
| Verificação de Peso        | Precisão de ±10 g no módulo de peso.                                         |

---

## Conclusão

O projeto demonstrou que é possível desenvolver um sistema ASRS de baixo custo e fácil implementação, utilizando componentes acessíveis como Arduino e ESP32. O sistema foi capaz de realizar movimentações precisas e integrar funcionalidades como autenticação RFID e verificação de peso.

Para futuras melhorias, sugere-se:
- Aumentar a capacidade de carga do sistema.
- Implementar um sistema de feedback para correção de erros de posicionamento.
- Desenvolver uma interface gráfica mais intuitiva.

