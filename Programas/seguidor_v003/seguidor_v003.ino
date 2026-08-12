async Task Main(){
    IO.OpenConsole();

    double limiteDeLuz = 0.3; // valor de corte (ajuste conforme o sensor)

    while (true){
        await Time.Delay(1); 

        // --- Leitura dos sensores de cor (linha) ---
        string sensorE1 = Bot.GetComponent<ColorSensor>("lc1").Analog.ToString(); // extremo esquerdo
        string sensorE  = Bot.GetComponent<ColorSensor>("lc").Analog.ToString();  // esquerdo
        string sensorC  = Bot.GetComponent<ColorSensor>("cc").Analog.ToString();  // central
        string sensorD  = Bot.GetComponent<ColorSensor>("rc").Analog.ToString();  // direito
        string sensorD1 = Bot.GetComponent<ColorSensor>("rc1").Analog.ToString(); // extremo direito

        // --- Leitura de luminosidade geral ---
        double brilhoAmbiente = Bot.GetComponent<ColorSensor>("cc").Analog.Brightness;

        // >>>> Condição extra: modo busca se pouca luz <<<<
        if (brilhoAmbiente < limiteDeLuz) {
            Bot.GetComponent<Light>("led").TurnOn(new Color(255, 140, 0)); // LED laranja para indicar modo busca
            await Direita90(250); // Exemplo: gira até achar luz/linha
            continue; // pula para próxima iteração sem executar as rotinas normais
        }

        // >>>> Restante do seguidor de linha normal <<<<

        // Linha bem centralizada
        if ((sensorC == "Preto") && (sensorE != "Preto") && (sensorD != "Preto")){
            Bot.GetComponent<Light>("led").TurnOn(new Color(0, 255, 0));
            await Frente(180);
        }
        // Pequeno desvio à esquerda
        else if ((sensorE == "Preto") && (sensorC != "Preto")){
            Bot.GetComponent<Light>("led").TurnOn(new Color(255, 0, 0));
            await Esquerda(180);
        }
        // Pequeno desvio à direita
        else if ((sensorD == "Preto") && (sensorC != "Preto")){
            Bot.GetComponent<Light>("led").TurnOn(new Color(0, 0, 255));
            await Direita(180);
        }
        // Curva 90 à esquerda 
        else if ((sensorE1 == "Preto") || (sensorE == "Preto")){
            await Frente(0);
            await Esquerda90(350);
            Bot.GetComponent<Light>("led").TurnOn(new Color(200, 50, 200));
        }
        // Encruzilhada
        else if ((sensorE == "Preto") && (sensorD == "Preto") && (sensorC == "Preto") && (sensorE1 == "Preto") && (sensorD1 == "Preto")){
            Bot.GetComponent<Light>("led").TurnOn(new Color(255, 255, 0));
            await Frente(180);
        }
        // Curva 90 à direita
        else if ((sensorD1 == "Preto") || (sensorD == "Preto")){
            await Frente(0);
            await Direita90(350);
            Bot.GetComponent<Light>("led").TurnOn(new Color(50, 200, 200));
        }
        // Condição verde
        else if ((sensorE1 == "Verde") || (sensorE == "Verde") || (sensorC == "Verde")){
            Bot.GetComponent<Light>("led").TurnOn(new Color(100,149,237));
            await Esquerda90(350);
        }
        // Marca final
        else if ((sensorC == "Cinza") && (sensorE == "Cinza") && (sensorD == "Cinza") && (sensorE1 == "Cinza") && (sensorD1 == "Cinza")){
            Bot.GetComponent<Light>("led").TurnOn(new Color(0,255,255));
            await Frente(0);
        }
    }
}
async Task Main(){
    IO.OpenConsole();

    double limiteParede = 0.15; // em metros, por exemplo (ajuste conforme seu sensor)

    while (true){
        await Time.Delay(1);

        // Lê os sensores de distância
        double frente = Bot.GetComponent<DistanceSensor>("frente").Analog.Distance;
        double esquerda = Bot.GetComponent<DistanceSensor>("esquerda").Analog.Distance;
        double direita = Bot.GetComponent<DistanceSensor>("direita").Analog.Distance;

        // >>> Condição: parede detectada na frente <<<
        if (frente < limiteParede){
            Bot.GetComponent<Light>("led").TurnOn(new Color(255, 140, 0)); // LED laranja = modo desvio
            await Frente(0); // para
            await Direita90(250); // desvia pela direita
            continue; // não segue rotina normal
        }

        // >>> Condição: parede lateral esquerda <<<
        if (esquerda < limiteParede){
            Bot.GetComponent<Light>("led").TurnOn(new Color(255, 0, 0)); // vermelho = parede à esquerda
            await Direita(180); 
            continue;
        }

        // >>> Condição: parede lateral direita <<<
        if (direita < limiteParede){
            Bot.GetComponent<Light>("led").TurnOn(new Color(0, 0, 255)); // azul = parede à direita
            await Esquerda(180);
            continue;
        }

        // >>> Se não tem parede, segue rotina normal de linha <<<
        // (toda aquela lógica que você já escreveu com sensores de cor)
    }
}
