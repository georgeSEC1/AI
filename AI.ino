//TODO, record series of microphone datums, reshape neural network, squeeze datums into sizable array with mean variables, convert to binary segments, trim excess data, nn training, recognise sounds, do KB logic, implement NN accuracy, autolearning optimisation and auto categorisation, add advice text output, sentiment analysis for guidance(commands)
/*
  www.aifes.ai
  https://github.com/Fraunhofer-IMS/AIfES_for_Arduino
  Copyright (C) 2020-2022  Fraunhofer Institute for Microelectronic Circuits and Systems.
  All rights reserved.

  AIfES is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  AIfES XOR Inference demo
  --------------------

  Versions:
    1.0.0   Initial version
    1.1.0   Change tensor initialization for new AIfES version
  
  The sketch shows an example of how the inference of an already trained network is performed. 
  In the concrete example, a neural network was trained to map an XOR gate. 
  The neural network was trained in Keras and the configuration including the weights was imported into AIfES. 
  The network structure is 2-3(Sigmoid)-1(Sigmoid) and Sigmoid is used as activation function.
  The calculation is done in float 32.
  
  XOR truth table
  Input    Output
  0   0    0
  0   1    1
  1   0    1
  1   1    0
  
  Input data in the example (0.0 | 1.0)
  
  
  Tested on:
    Arduino UNO
    Arduino Nano
    Arduino Nano 33 BLE Sense (Cortex M4)
    Seeeduino XIAO (Cortex M0+)
   
*/

//Serial keyword: "inference"

#include <aifes.h>  // include the AIfES libary

void setup() {
  Serial.begin(115200);  //115200 baud rate (If necessary, change in the serial monitor)
  Serial.println(F("Type >inference< to start AI"));
}

void loop() {
  Serial.println(F("AIfES:"));
  Serial.println(F(""));
  Serial.print(F("rand test: "));
  Serial.println(rand());
  Serial.println();

  uint32_t i;

  // Tensor for the training data
  uint16_t input_shape[] = { 4, 2 };  // Definition of the input shape
  // Corresponds to the XOR truth table
  float input_data[4 * 2] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
  };
  // Two dimensional(2D)array example
  // The "Serial.print" output must then be modified
  /*
      float input_data[4][2] = {
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f}
      };
      */
  aitensor_t input_tensor = AITENSOR_2D_F32(input_shape, input_data);  // Creation of the input AIfES tensor with two dimensions and data type F32 (float32)

  // Tensor for the target data
  uint16_t target_shape[] = { 4, 1 };  // Definition of the input shape
  // Corresponds to the XOR truth table
  float target_data[4 * 1] = {
    0.0f,
    1.0f,
    1.0f,
    0.0f
  };
  aitensor_t target_tensor = AITENSOR_2D_F32(target_shape, target_data);  // Assign the target_data array to the tensor. It expects a pointer to the array where the data is stored

  // Empty tensor for the output data (result after training).
  // Same configuration as for the target tensor
  uint16_t output_shape[] = { 4, 1 };
  float output_data[4 * 1];
  aitensor_t output_tensor = AITENSOR_2D_F32(output_shape, output_data);

  // ---------------------------------- Layer definition ---------------------------------------

  uint16_t input_layer_shape[] = { 1, 2 };                                                                          // Definition of the input layer shape (Must fit to the input tensor)
  ailayer_input_f32_t input_layer = AILAYER_INPUT_F32_A(/*input dimension=*/2, /*input shape=*/input_layer_shape);  // Creation of the AIfES input layer
  ailayer_dense_f32_t dense_layer_1 = AILAYER_DENSE_F32_A(/*neurons=*/3);                                           // Creation of the AIfES hidden dense layer with 3 neurons
  ailayer_sigmoid_f32_t sigmoid_layer_1 = AILAYER_SIGMOID_F32_A();                                                  // Hidden activation function
  ailayer_dense_f32_t dense_layer_2 = AILAYER_DENSE_F32_A(/*neurons=*/1);                                           // Creation of the AIfES output dense layer with 1 neuron
  ailayer_sigmoid_f32_t sigmoid_layer_2 = AILAYER_SIGMOID_F32_A();                                                  // Output activation function

  ailoss_mse_t mse_loss;  //Loss: mean squared error

  // --------------------------- Define the structure of the model ----------------------------

  aimodel_t model;  // AIfES model
  ailayer_t *x;     // Layer object from AIfES to connect the layers

  // Connect the layers to an AIfES model
  model.input_layer = ailayer_input_f32_default(&input_layer);
  x = ailayer_dense_f32_default(&dense_layer_1, model.input_layer);
  x = ailayer_sigmoid_f32_default(&sigmoid_layer_1, x);
  x = ailayer_dense_f32_default(&dense_layer_2, x);
  x = ailayer_sigmoid_f32_default(&sigmoid_layer_2, x);
  model.output_layer = x;

  // Add the loss to the AIfES model
  model.loss = ailoss_mse_f32_default(&mse_loss, model.output_layer);

  aialgo_compile_model(&model);  // Compile the AIfES model

  // ------------------------------------- Print the model structure ------------------------------------

  Serial.println(F("-------------- Model structure ---------------"));
  Serial.println(F("Layer:"));
  aialgo_print_model_structure(&model);
  Serial.print(F("\nLoss: "));
  aialgo_print_loss_specs(model.loss);
  Serial.println(F("\n----------------------------------------------\n"));

  // ------------------------------- Allocate memory for the parameters of the model ------------------------------

  uint32_t parameter_memory_size = aialgo_sizeof_parameter_memory(&model);
  Serial.print(F("Required memory for parameter (Weights, Biases): "));
  Serial.print(parameter_memory_size);
  Serial.print(F(" bytes"));
  Serial.println();
  byte *parameter_memory = (byte *)malloc(parameter_memory_size);

  // Distribute the memory for the trainable parameters of the model
  aialgo_distribute_parameter_memory(&model, parameter_memory, parameter_memory_size);

  // ------------------------------- Initialize the parameters ------------------------------

  // Recommended weight initialization
  aimath_f32_default_init_glorot_uniform(&dense_layer_1.weights);
  aimath_f32_default_init_zeros(&dense_layer_1.bias);

  aimath_f32_default_init_glorot_uniform(&dense_layer_2.weights);
  aimath_f32_default_init_zeros(&dense_layer_2.bias);

  /* Another way of initializing the weights
      // Random weights in the value range from -2 to +2
      // The value range of the weights was chosen large, so that learning success is not always given ;)
      float max = 2.0;
      float min = -2.0;
      aimath_f32_default_tensor_init_uniform(&dense_layer_1.weights,max,min);
      aimath_f32_default_tensor_init_uniform(&dense_layer_1.bias,max,min);
      aimath_f32_default_tensor_init_uniform(&dense_layer_2.weights,max,min);
      aimath_f32_default_tensor_init_uniform(&dense_layer_2.bias,max,min);
      */

  // -------------------------------- Define the optimizer for training ---------------------

  // Adam optimizer
  aiopti_adam_f32_t adam_opti = AIOPTI_ADAM_F32(/*learning rate=*/0.1f, /*beta_1=*/0.9f, /*beta_2=*/0.999f, /*eps=*/1e-7);
  aiopti_t *optimizer = aiopti_adam_f32_default(&adam_opti);  // Initialize the optimizer

  // Alternative 1: Stochastic Gradient Descent optimizer (SGD)
  //aiopti_sgd_f32_t sgd_opti = AIOPTI_SGD_F32(/*learning rate=*/ 0.1f);
  //aiopti_t *optimizer = aiopti_sgd_f32_default(&sgd_opti);

  // Alternative 2: Stochastic Gradient Descent optimizer (SGD) with momentum
  //aiopti_sgd_f32_t sgd_opti = AIOPTI_SGD_WITH_MOMENTUM_F32(/*learning rate=*/ 0.1f, /*momentum=*/ 0.9f);
  //aiopti_t *optimizer = aiopti_sgd_f32_default(&sgd_opti);

  // -------------------------------- Allocate and schedule the working memory for training ---------

  uint32_t memory_size = aialgo_sizeof_training_memory(&model, optimizer);
  Serial.print(F("Required memory for the training (Intermediate results, gradients, optimization memory): "));
  Serial.print(memory_size);
  Serial.print(F(" bytes"));
  Serial.println();
  byte *memory_ptr = (byte *)malloc(memory_size);

  if (memory_ptr == 0) {
    Serial.println(F("ERROR: Not enough memory (RAM) available for training! Try to use another optimizer (e.g. SGD) or make your net smaller."));
    while (1)
      ;
  }

  // Schedule the memory over the model
  aialgo_schedule_training_memory(&model, optimizer, memory_ptr, memory_size);

  // IMPORTANT: Initialize the AIfES model before training
  aialgo_init_model_for_training(&model, optimizer);

  // --------------------------------- Print the result before training ----------------------------------

  uint32_t input_counter = 0;  // Counter to print the inputs/training data

  // Do the inference before training
  aialgo_inference_model(&model, &input_tensor, &output_tensor);

  Serial.println();
  Serial.println(F("Before training:"));
  Serial.println(F("Results:"));
  Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));

  for (i = 0; i < 4; i++) {
    Serial.print(input_data[input_counter]);
    //Serial.print(((float* ) input_tensor.data)[i]); //Alternative print for the tensor
    input_counter++;
    Serial.print(F("\t\t"));
    Serial.print(input_data[input_counter]);
    input_counter++;
    Serial.print(F("\t\t"));
    Serial.print(target_data[i]);
    Serial.print(F("\t\t"));
    Serial.println(output_data[i]);
    //Serial.println(((float* ) output_tensor.data)[i]); //Alternative print for the tensor
  }

  // ------------------------------------- Training configuration ------------------------------------

  uint32_t batch_size = 4;  // Configuration tip: ADAM=4   / SGD=1
  uint16_t epochs = 100;    // Configuration tip: ADAM=100 / SGD=550
  uint16_t print_interval = 10;

  Serial.println(F("\n------------ Training configuration ----------"));
  Serial.print(F("Epochs: "));
  Serial.print(epochs);
  Serial.print(F(" (Print loss every "));
  Serial.print(print_interval);
  Serial.println(F(" epochs)"));
  Serial.print(F("Batch size: "));
  Serial.println(batch_size);
  Serial.print(F("Optimizer: "));
  aialgo_print_optimizer_specs(optimizer);
  Serial.println(F("\n----------------------------------------------\n"));

  // ------------------------------------- Run the training ------------------------------------

  Serial.println(F("Start training"));
  float loss;
  for (i = 0; i < epochs; i++) {
    // One epoch of training. Iterates through the whole data once
    aialgo_train_model(&model, &input_tensor, &target_tensor, optimizer, batch_size);

    // Calculate and print loss every print_interval epochs
    if (i % print_interval == 0) {
      aialgo_calc_loss_model_f32(&model, &input_tensor, &target_tensor, &loss);
      Serial.print(F("Epoch: "));
      Serial.print(i);
      Serial.print(F(" Loss: "));
      Serial.println(loss);
    }
  }
  Serial.println(F("Finished training"));
  Serial.println(F("Type >inference< to begin AI"));
while(true){
  while (Serial.available() > 0) {
    String str = Serial.readString();
    if (str.indexOf("inference") > -1) {  // Keyword "inference"
      Serial.println();
      // ----------------------------------------- Evaluate the trained model --------------------------

      // Do the inference after training
      aialgo_inference_model(&model, &input_tensor, &output_tensor);

      Serial.println(F(""));
      Serial.println(F("After training:"));
      Serial.println(F("Results:"));
      Serial.println(F("input 1:\tinput 2:\treal output:\tcalculated output:"));

      input_counter = 0;

      for (i = 0; i < 4; i++) {
        Serial.print(input_data[input_counter]);
        //Serial.print(((float* ) input_tensor.data)[i]); //Alternative print for the tensor
        input_counter++;
        Serial.print(F("\t\t"));
        Serial.print(input_data[input_counter]);
        input_counter++;
        Serial.print(F("\t\t"));
        Serial.print(target_data[i]);
        Serial.print(F("\t\t"));
        Serial.println(output_data[i]);
        //Serial.println(((float* ) output_tensor.data)[i]); //Alternative print for the tensor
      }

      // How to print the weights example
      // Serial.println(F("Dense 1 - Weights:"));
      // print_aitensor(&dense_layer_1.weights);
      // Serial.println(F("Dense 1 - Bias:"));
      // print_aitensor(&dense_layer_1.bias);

      Serial.println();
      Serial.println(F("A learning success is not guaranteed"));
      Serial.println(F("The weights were recalled"));
      Serial.println(F("You can repeat the inference with >inference<"));

      free(parameter_memory);
      free(memory_ptr);
    }
  }
}
}
