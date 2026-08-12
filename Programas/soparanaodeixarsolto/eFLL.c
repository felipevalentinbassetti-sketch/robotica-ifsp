// ===============================
// ANTECEDENTES (PARTE "SE" DA REGRA)
// ===============================

// Cria um antecedente fuzzy vazio
// Esse objeto representa a condição "SE ..."
FuzzyRuleAntecedent* ifDistanceSmall = new FuzzyRuleAntecedent();

// joinSingle(): associa um único conjunto fuzzy ao antecedente
// Exemplo lógico: SE distance É small
ifDistanceSmall->joinSingle(small);


// Cria um antecedente composto
// Ele será usado para combinar duas condições
FuzzyRuleAntecedent* ifTemperatureHotAndPressureHight = new FuzzyRuleAntecedent();

// joinWithAND(): combina dois conjuntos fuzzy usando o operador lógico AND
// Exemplo lógico: SE temperature É hot E pressure É hight
ifTemperatureHotAndPressureHight->joinWithAND(hot, hight);


// Outro antecedente composto
FuzzyRuleAntecedent* ifTemperatureHotOrPressureHight = new FuzzyRuleAntecedent();

// joinWithOR(): combina dois conjuntos fuzzy usando o operador lógico OR
// Exemplo lógico: SE temperature É hot OU pressure É hight
ifTemperatureHotOrPressureHight->joinWithOR(hot, hight);

// Combina um conjunto fuzzy com um antecedente usando AND
bool joinWithAND(FuzzySet* fuzzySet, FuzzyRuleAntecedent* fuzzyRuleAntecedent);

// Combina um antecedente com um conjunto fuzzy usando AND
bool joinWithAND(FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzySet* fuzzySet);

// Combina um conjunto fuzzy com um antecedente usando OR
bool joinWithOR(FuzzySet* fuzzySet, FuzzyRuleAntecedent* fuzzyRuleAntecedent);

// Combina um antecedente com um conjunto fuzzy usando OR
bool joinWithOR(FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzySet* fuzzySet);

// Combina dois antecedentes usando AND
// Permite regras mais complexas
bool joinWithAND(FuzzyRuleAntecedent* fuzzyRuleAntecedent1,
                FuzzyRuleAntecedent* fuzzyRuleAntecedent2);

// Combina dois antecedentes usando OR
bool joinWithOR(FuzzyRuleAntecedent* fuzzyRuleAntecedent1,
                FuzzyRuleAntecedent* fuzzyRuleAntecedent2);

// Cria um antecedente composto
// Representa: speed É hight AND distance É small
FuzzyRuleAntecedent* speedHightAndDistanceSmall = new FuzzyRuleAntecedent();

// Combina os dois conjuntos fuzzy com AND
speedHightAndDistanceSmall->joinWithAND(hight, small);

// Cria um antecedente simples
// Representa: fuel É low
FuzzyRuleAntecedent* fuelLow = new FuzzyRuleAntecedent();

// Associa diretamente o conjunto fuzzy "low"
fuelLow->joinSingle(low);

// Este antecedente final junta dois antecedentes menores
// Ele será usado diretamente na regra fuzzy
FuzzyRuleAntecedent* ifSpeedHightAndDistanceSmallOrFuelLow =
    new FuzzyRuleAntecedent();

// Combina:
// (speed É hight AND distance É small) OR (fuel É low)
ifSpeedHightAndDistanceSmallOrFuelLow->joinWithOR(
    speedHightAndDistanceSmall,
    fuelLow
);





// ===============================
// CONSEQUENTE (PARTE "ENTÃO")
// ===============================

// Cria o consequente da regra
// Representa a parte "ENTÃO ..."
FuzzyRuleConsequent* thenSpeedSlow = new FuzzyRuleConsequent();

// addOutput(): define qual conjunto fuzzy será ativado como saída
// Exemplo: ENTÃO speed É slow
thenSpeedSlow->addOutput(slow);

// Cria a regra fuzzy completa
// Parâmetros:
// 1) ID da regra (2)
// 2) Antecedente (SE ...)
// 3) Consequente (ENTÃO ...)
FuzzyRule* fuzzyRule =
    new FuzzyRule(2, ifDistanceSmall, thenSpeedSlow);


// Cria um consequente com múltiplas saídas
// Uma regra pode afetar mais de uma variável de saída
FuzzyRuleConsequent* thenSpeedSmallAndFeedSmall =
    new FuzzyRuleConsequent();

// Define as saídas fuzzy ativadas
// ENTÃO speed É small
thenSpeedSmallAndFeedSmall->addOutput(small);

// ENTÃO feed É tine
thenSpeedSmallAndFeedSmall->addOutput(tine);


// Regra fuzzy completa usando antecedente composto
FuzzyRule* fuzzyRule =
    new FuzzyRule(
        2,
        ifSpeedHightAndDistanceSmallOrFuelLow,
        thenSpeedSmallAndFeedSmall
);

