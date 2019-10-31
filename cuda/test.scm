(init_device_mem 9)
(dim 0 600 10) ; test labels
(dim 1 600 (* 28 28)) ; input data
(dim 2 (* 28 28) 500) ; first layer of weights
(dim 3 600 500) ; first layer of activations
(dim 4 500 10); second layer of weights
(dim 5 600 10); second layer of activations
(dim 6 600 1); storage for partition function
(dim 7 600 10); output layer loss
(dim 8 600 500); hidden layer loss

(import_labels "train-labels-idx1-ubyte" 600 0)
(import_images "train-images-idx3-ubyte" 600 1)

(init_weights 2)
(init_weights 4)

(display_row 1 0 28 28)
(check_logit 0 0)
(display_row 3 0 20 25)

(m* 1 2 3)
(rect 3)
(m* 3 4 5)
(partf 5 6)
(softm 5 6)
(crosse 5 0 7)

