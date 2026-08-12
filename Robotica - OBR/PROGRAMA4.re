#COR
#FRENTE, ESQUERDA, DIREITA E TRAS

#FRENTE
tarefa FRENTE com numero velocidade = 100{
	TravarMotor("motorFD", falso)
	TravarMotor("motorFE", falso)
	TravarMotor("motorTD", falso)
	TravarMotor("motorTE", falso)
	Motor("motorFD", velocidade)
	Motor("motorFE", velocidade)
	Motor("motorTD", velocidade)
	Motor("motorTE", velocidade)
}

#ESQUERDA
tarefa ESQUERDA com numero velocidade = 100{
	TravarMotor("motorFD", falso)
	TravarMotor("motorFE", verdadeiro)
	TravarMotor("motorTD", falso)
	TravarMotor("motorTE", verdadeiro)
	Motor("motorFD", velocidade)
	Motor("motorTD", velocidade)
	Motor("motorFE", 0-velocidade)
	Motor("motorTE", 0-velocidade)
}

#DIREITA
tarefa DIREITA com numero velocidade = 100{
	TravarMotor("motorFD", verdadeiro)
	TravarMotor("motorFE", falso)
	TravarMotor("motorTD", verdadeiro)
	TravarMotor("motorTE", falso)
	Motor("motorFE", velocidade)
	Motor("motorTE", velocidade)
	Motor("motorFD", 0-velocidade)
	Motor("motorTD", 0-velocidade)
}

#TRAS
tarefa TRAS com numero velocidade = 100{
	TravarMotor("motorFD", falso)
	TravarMotor("motorFE", falso)
	TravarMotor("motorTD", falso)
	TravarMotor("motorTE", falso)
	Motor("motorFD", 0-velocidade)
	Motor("motorFE", 0-velocidade)
	Motor("motorTD", 0-velocidade)
	Motor("motorTE", 0-velocidade)
}

#PARAR
tarefa PARAR com numero velocidade = 1{
	TravarMotor("motorFD", verdadeiro)
	TravarMotor("motorFE", verdadeiro)
	TravarMotor("motorTD", verdadeiro)
	TravarMotor("motorTE", verdadeiro)
    Motor("motorFD", velocidade)
	Motor("motorFE", velocidade)
	Motor("motorTD", velocidade)
	Motor("motorTE", velocidade)
}

inicio
    AbrirConsole()
	enquanto(verdadeiro) farei {
        EscreverLinha(Cor("sensorD"))
        EscreverLinha(Cor("sensorED"))
        EscreverLinha(Cor("sensorE"))
        EscreverLinha(Cor("sensorEE"))
		Esperar(1)
		se(Cor("sensorD") == "Preto" ou Cor("sensorED") == "Preto") entao {
			DIREITA(200)
		} senao se(Cor("sensorE") == "Preto" ou Cor("sensorEE") == "Preto") entao {
			ESQUERDA(200)
		} senao se(Cor("sensorD") == "Verde" ou Cor("sensorED") == "Verde") entao {
			DIREITA(200)
            Esperar(1000)
        }  senao se(Cor("sensorE") == "Verde" ou Cor("sensorEE") == "Verde") entao {
			ESQUERDA(200)
            Esperar(1000)
        } senao {
			FRENTE(200)
        }
}
fim