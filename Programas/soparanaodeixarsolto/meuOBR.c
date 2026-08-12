async Task Frente(double velocidade = 200){
    Bot.GetComponent<Servomotor>("l").Locked = false;
    Bot.GetComponent<Servomotor>("r").Locked = false;
    Bot.GetComponent<Servomotor>("ll").Locked = false;
    Bot.GetComponent<Servomotor>("rr").Locked = false;

    Bot.GetComponent<Servomotor>("l").Apply(Math.Abs(velocidade), velocidade);
    Bot.GetComponent<Servomotor>("r").Apply(Math.Abs(velocidade), velocidade);
    Bot.GetComponent<Servomotor>("ll").Apply(Math.Abs(velocidade), velocidade);
    Bot.GetComponent<Servomotor>("rr").Apply(Math.Abs(velocidade), velocidade);
}

async Task Direita(double velocidade = 200){
    Bot.GetComponent<Servomotor>("l").Locked = false;
    Bot.GetComponent<Servomotor>("r").Locked = false;
    Bot.GetComponent<Servomotor>("ll").Locked = false;
    Bot.GetComponent<Servomotor>("rr").Locked = false;

    Bot.GetComponent<Servomotor>("l").Apply(Math.Abs(velocidade * 2), velocidade * 2);
    Bot.GetComponent<Servomotor>("r").Apply(Math.Abs(velocidade * 1), velocidade * 1);
    Bot.GetComponent<Servomotor>("ll").Apply(Math.Abs(velocidade * 2), velocidade * 2);
    Bot.GetComponent<Servomotor>("rr").Apply(Math.Abs(velocidade * 1), velocidade * 1);
}

async Task Direita90(double velocidade = 200){
    Bot.GetComponent<Servomotor>("l").Locked = false;
    Bot.GetComponent<Servomotor>("r").Locked = false;
    Bot.GetComponent<Servomotor>("ll").Locked = false;
    Bot.GetComponent<Servomotor>("rr").Locked = false;

    Bot.GetComponent<Servomotor>("l").Apply(Math.Abs(velocidade * 2), velocidade * 2);
    Bot.GetComponent<Servomotor>("r").Apply(Math.Abs(velocidade * -1), velocidade * -1);
    Bot.GetComponent<Servomotor>("ll").Apply(Math.Abs(velocidade * 2), velocidade * 2);
    Bot.GetComponent<Servomotor>("rr").Apply(Math.Abs(velocidade * -1), velocidade * -1);
}

async Task Esquerda(double velocidade = 200){
    Bot.GetComponent<Servomotor>("l").Locked = false;
    Bot.GetComponent<Servomotor>("r").Locked = false;
    Bot.GetComponent<Servomotor>("ll").Locked = false;
    Bot.GetComponent<Servomotor>("rr").Locked = false;

    Bot.GetComponent<Servomotor>("l").Apply(Math.Abs(velocidade * 1), velocidade * 1);
    Bot.GetComponent<Servomotor>("r").Apply(Math.Abs(velocidade * 2), velocidade * 2);
    Bot.GetComponent<Servomotor>("ll").Apply(Math.Abs(velocidade * 1), velocidade * 1);
    Bot.GetComponent<Servomotor>("rr").Apply(Math.Abs(velocidade * 2), velocidade * 2);
}

async Task Esquerda90(double velocidade = 200){
    Bot.GetComponent<Servomotor>("l").Locked = false;
    Bot.GetComponent<Servomotor>("r").Locked = false;
    Bot.GetComponent<Servomotor>("ll").Locked = false;
    Bot.GetComponent<Servomotor>("rr").Locked = false;

    Bot.GetComponent<Servomotor>("l").Apply(Math.Abs(velocidade * -1), velocidade * -1);
    Bot.GetComponent<Servomotor>("r").Apply(Math.Abs(velocidade * 2), velocidade * 2);
    Bot.GetComponent<Servomotor>("ll").Apply(Math.Abs(velocidade * -1), velocidade * -1);
    Bot.GetComponent<Servomotor>("rr").Apply(Math.Abs(velocidade * 2), velocidade * 2);
}

async Task LedFinal(){
    for (int i = 1; i <= 5; i++){
        Bot.GetComponent<Light>("led").TurnOn(new Color(255, 0, 0));
        await Time.Delay(50);
        Bot.GetComponent<Light>("led").TurnOn(new Color(0, 255, 0));
        await Time.Delay(50);
        Bot.GetComponent<Light>("led").TurnOn(new Color(0, 0, 255));
        await Time.Delay(50);
    }
}
async Task Main(){
    IO.OpenConsole();

    while (true){
        await Time.Delay(1); 
;

        string sensorE1 = Bot.GetComponent<ColorSensor>("lc1").Analog.ToString(); // extremo esquerdo
        string sensorE  = Bot.GetComponent<ColorSensor>("lc").Analog.ToString();  // esquerdo
        string sensorC  = Bot.GetComponent<ColorSensor>("cc").Analog.ToString();  // central
        string sensorD  = Bot.GetComponent<ColorSensor>("rc").Analog.ToString();  // direito
        string sensorD1 = Bot.GetComponent<ColorSensor>("rc1").Analog.ToString(); // extremo direito

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
        // Encruzilhada (vários sensores detectando linha ao mesmo tempo)
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

        else if ((sensorE1 == "Verde") || (sensorE == "Verde") || (sensorC == "Verde")){
                Bot.GetComponent<Light>("led").TurnOn(new Color(100,149,237));
                await Esquerda90(350);
        }
        // Marca final (linha vermelha em qualquer sensor)
        else if ((sensorC == "Cinza") && (sensorE == "Cinza") && (sensorD == "Cinza") && (sensorE1 ==         "Cinza") && (sensorD1 == "Cinza")){
            Bot.GetComponent<Light>("led").TurnOn(new Color(0,255,255));
            await Frente(0);
        }
        // Fora da linha
        /*else if ((sensorC != "Preto") && (sensorE != "Preto") && (sensorD != "Preto") &&
                 (sensorE1 != "Preto") && (sensorD1 != "Preto")){
            await Frente(-70); // dá uma ré pequena para tentar se reposicionar
            Bot.GetComponent<Light>("led").TurnOn(new Color(0, 0, 0));
        }*/
    }
}