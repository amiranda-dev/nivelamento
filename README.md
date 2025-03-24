<h1>Embarcatech - SISTEMA DE DETECÇÃO VISUAL DE CAMPAINHA PARA DEFICIENTE 
AUDITIVO</h1>

   # Descrição
   <p>Este projeto foi desenvolvido para melhorar a acessibilidade de pessoas 
com deficiência auditiva, permitindo que percebam quando uma campainha está 
tocando. O sistema utiliza um microfone para detectar o som da campainha e 
fornece alertas visuais acendendo um LED, exibindo a mensagem "Campainha 
Tocando" no display OLED e apresentando um ícone de uma casa na matriz de 
LEDs 5x5. Além disso, um botão A permite que o usuário desligue manualmente 
os LEDs e o display ou esperar 3 minutos para que o alerta visual desligue tudo 
automaticamente. O motivo disse é para verificar se o dono da casa o recebeu 
e assim desligar o alerta visual ou esperar ao tempo citado acima para caso o 
dono exceda do tempo do não atendimento.</p>

   # Hardware Utilizado

- **Microfone**: Conectado ao pino 28.
- **LED Vermelho**: Conectado ao pino 13.
- **Botão A**: Conectado ao pino 5.
- **Matriz de LEDs 5x5**: Controlada por PIO e conectada ao pino 7.
- **Display OLED (SSD1306)**: Controlado via I2C com conexões nos pinos SDA (14) e SCL (15).
- **Raspberry Pi Pico**: Microcontrolador utilizado no projeto.

## Funcionalidades

1. **Detecção de Pico de Som**: O microfone conectado ao ADC da Raspberry Pi Pico detecta picos de som, como o toque de uma campainha.
   
2. **Feedback Visual**:
   - Quando o som da campainha é detectado, o LED vermelho acende.
   - A mensagem "Campainha Tocando" é mostrada no display OLED.
   - A matriz de LEDs 5x5 exibe uma carinha feliz.

3. **Controle do LED via Botão**: O LED pode ser desligado pressionando o botão A. O display OLED e a matriz de LEDs também são apagados ao pressionar o botão.

4. **Filtro de Média Móvel**: Um filtro de média móvel é aplicado para suavizar as leituras do microfone, tornando a detecção de picos mais precisa.

## Estrutura do Código

- **Configuração de Pinos**: Define os pinos GPIO para os dispositivos conectados (microfone, LEDs, botão e display).
- **Inicialização do ADC**: Configura o ADC para a leitura dos valores analógicos do microfone.
- **Controle do LED**: Funções responsáveis por ligar e desligar o LED vermelho.
- **Controle do Display OLED**: Funções que inicializam e atualizam o conteúdo do display.
- **Controle da Matriz de LEDs**: Funções para exibir padrões de LEDs na matriz 5x5.
- **Detecção de Pico de Som**: Implementa a lógica de detecção de som utilizando o filtro de média móvel.
- **Interrupção do Botão**: Configura uma interrupção para o botão A, que apaga os LEDs e o display quando pressionado ou esperar 3 minutos.

## Detalhamento das Funcionalidades

### Detecção de Pico de Som
O sistema lê os valores do microfone através do ADC de forma contínua. Esses valores são suavizados por meio de um filtro de média móvel, e quando o valor suavizado excede um limite pré-definido, o sistema detecta um pico de som. Com isso:

- O LED vermelho é aceso.
- A mensagem "Campainha Tocando" é exibida no display OLED.
- A matriz de LEDs 5x5 mostra uma carinha feliz.

### Interrupção do Botão
O botão A (conectado ao GPIO5) gera uma interrupção ao ser pressionado. Essa interrupção desliga o LED, apaga a mensagem no display OLED e também desliga a matriz de LEDs.

## Como Rodar o Projeto

1. **Instalar o SDK da Raspberry Pi Pico**: Antes de compilar o código, configure o SDK da Raspberry Pi Pico seguindo o guia oficial.
   
2. **Compilar o Código**: Use o CMake para compilar o projeto. Certifique-se de que o arquivo `CMakeLists.txt` esteja configurado corretamente para os componentes utilizados.

3. **Carregar o Código na Raspberry Pi Pico**: Após a compilação, carregue o código no microcontrolador conectando a Raspberry Pi Pico via USB (pressione o botão BOOTSEL ao conectar).

4. **Executar o Projeto**: Assim que o código for carregado, o sistema iniciará a detecção de som e exibirá os feedbacks visuais correspondentes.

## Dependências

- **SSD1306**: Biblioteca para controlar o display OLED via I2C.
- **PIO (Programmable Input/Output)**: Utilizado para controlar a matriz de LEDs WS2812.
- **ADC**: Biblioteca para leitura do Conversor Analógico-Digital (ADC).

Esse projeto combina detecção de som, controle de display, matriz de LEDs e botões para um sistema interativo com resposta visual.
<h3>Vídeo de Demostração</h3>
<p>YouTube: https://youtu.be/_bO93-7aTnU</p>
