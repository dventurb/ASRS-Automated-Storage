# Automated Storage and Retrieval System (ASRS)

This repo covers the development of a low-cost **Automated Storage and Retrieval System (ASRS)**, using stepper motors, ESP32, Arduino Mega 2560 and some other electronic components. The system is controlled with a FIFO (First-In First-Out) Algorithm and RFID authetication. It also integrates a load cell to save the weight of the material, and storing it alongside the RFID Card.

<div align="center">
    <img src="https://imgur.com/B0eKkpb.png" alt="script" width="300">
</div>


## Estrutura do Repositório

- **/CAD**: STL and STEP files.
- **/Electronics**: Electrical schematic and diagrams.
- **/Software**: Source code for Arduino and ESP32.
- **/Pictures**: Images for analysis. 
- **/Docs**: Relevant documents.


## Estrutura Mecânica
The structure is primarily build from wood with some 3D-printed parts for the motors and axis. The Y-Axis use a lead screw for vertical movement, while the X-Axis and Z-Axis use timing belts for horizontal movement.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Mechanics/ASRS_MechanicalComponents.png" alt="script" width="300">
</div>

### **Mechanical Components**  

| **Component**              | **Quantity** | **Description**                                              |  
|----------------------------|-------------|--------------------------------------------------------------|  
| Lead screw (8mm, 50cm)     | 1           | Used for vertical movement on the Y-axis.                   |  
| Coupler                    | 1           | Connects the lead screw to the stepper motor.               |  
| Linear shafts (10mm, 50cm) | 4           | Provides smooth linear motion for the system.               |  
| Linear shafts (10mm, 20cm) | 2           | Supports movement in smaller sections of the system.        |  
| Linear shaft supports      | 8           | Holds the linear shafts in place.                           |  
| Linear bearing blocks      | 6           | Enables smooth sliding motion along the shafts.             |  
| GT2 timing belts           | 2           | Used for horizontal movement on the X and Z axes.           |  
| GT2 pulleys                | 2           | Compatible with the GT2 timing belts.                       |  
| 3D-printed supports        | X           | Used for mounting motors and mechanical components.         |  

---

### Funcionamento
**1. O ESP32 recebe autenticação via RFID e habilita o controlo do sistema.**

**2. O utilizador controla o sistema através dos Botões de Contato.**

**3. O ESP32 processa o algoritmo de FIFO e envia, via serial, o compartimento a ser tratado para o Arduino Mega.**

**4. O Arduino Mega interpreta os comandos e aciona os motores de passo para movimentação nos eixos X, Y e Z.**

**5. Os sensores garantem precisão e segurança nos movimentos.**

**6. O sistema registra a posição, o peso do material e a Tag RFID do utilizador.**

### Diagrama de Blocos
O Diagrama de Blocos oferece uma representação visual do fluxo de dados e controle dentro do sistema ASRS (Automated Storage and Retrieval System), organizando os principais componentes e suas interações. Ele é dividido em várias seções essenciais para o funcionamento do sistema, como Entradas, Saídas, Atuadores, Fonte de Alimentação e Microcontroladores.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Functional_Data_Block.png" alt="script" width="700">
</div>
   
### Circuito Eletrónico
Este é o esquema elétrico detalhado do sistema ASRS, desenvolvido no KiCad. Ele apresenta todas as conexões elétricas, incluindo componentes como ESP32, Arduino Mega, drivers de motor, sensores, fonte de alimentação e demais módulos eletrônicos utilizados no projeto.

[<img src="https://i.imgur.com/bHJxbk0.png" alt="Esquema" width="800">](https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Schematic_Colours.pdf)
    
Este diagrama fornece uma representação visual das ligações entre os principais componentes do ASRS. Ele facilita a montagem e a compreensão das conexões entre os microcontroladores, motores, sensores e módulos auxiliares. Ideal para consultas rápidas e montagem prática do circuito.

<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Electronics/Circuit_Connections.png" alt="script" width="1000">

### Programação e Fluxogramas

#### Algoritmo Queue (Fila)

O algoritmo Queue foi implementado para controlar a movimentação dos materiais dentro do sistema ASRS. A Queue segue a abordagem de FIFO (First In, First Out), onde o primeiro item a entrar será o primeiro a sair. Esse tipo de estrutura é ideal para o nosso sistema, pois garante que os materiais sejam armazenados e retirados de forma organizada, sem sobrecarregar nenhum compartimento.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Algorithm_Queue.png" alt="script" width="700">
</div>

#### Fluxogramas
O fluxograma abaixo ilustra o fluxo do programa no ESP32, com o algoritmo FIFO sendo utilizado para determinar a ordem de operação.

<div align="center">
<img src="https://github.com/dventurb/ASRS-Automated-Storage/blob/main/Pictures/Diagrams%20and%20Schematics/Flowchart_ESP32.png" alt="script" width="500">
</div>

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

