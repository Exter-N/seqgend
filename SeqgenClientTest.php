<?php

require_once __DIR__ . '/SeqgenClient.php';

$sgc = new SeqgenClient(65279);
var_dump($sgc->generate());
