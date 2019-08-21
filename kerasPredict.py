import numpy as np
import uproot
from keras import models
import tensorflow as tf
from keras import backend as K

import xgboost as xgb

def auc( true_labels, predictions, weights = None ):
    auc = tf.metrics.auc( true_labels,  predictions, weights = weights)[1]
    K.get_session().run( tf.local_variables_initializer() )
    return auc



class kerasModel():

    def __init__(self, file_name):
        self.model = models.load_model( file_name, custom_objects = {'auc':auc} )


    #return model prediction given a list of inputs
    def predict( self, highlevel_input, pfn_input ):
        pfn_array = np.asarray( pfn_input, dtype=float)
        pfn_array = pfn_array.reshape( (1,) + pfn_array.shape )

        highlevel_array = np.asarray(highlevel_input, dtype=float)
        highlevel_array = highlevel_array.reshape( (1,) + highlevel_array.shape )
        return float( self.model.predict( [pfn_array, highlevel_array] ) )


class xgboostModel():

    def __init__(self, file_name):
        self.model = xgb.Booster()
        self.model.load_model( file_name )

    def predict( self, highlevel_input ):
        highlevel_array = np.asarray( highlevel_input, dtype = float )
        highlevel_array = highlevel_array.reshape( (1,) + highlevel_array.shape )
        return float( self.model.predict(xgb.DMatrix( highlevel_array ) ) )


if __name__ == '__main__':
    pass
