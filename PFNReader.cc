#include "PFNReader.h"

//include c++ library classes
#include <stdexcept>
#include <iostream>

unsigned PFNReader::instanceCounter = 0;
boost::python::object PFNReader::pythonModule = boost::python::object();

PFNReader::PFNReader( const std::string& model_file_name, const unsigned highlevelInputShape, const std::pair<unsigned, unsigned>& pfnInputShape ):
    pfnShape( pfnInputShape ), highlevelShape( highlevelInputShape )
{
    if( instanceCounter == 0 ){
		initializePythonAPI();
		loadPythonModule( "kerasPredict" );
    }
    ++instanceCounter;

    // if pfnInputShape is {0, 0}, the model we want to read is a BDT model which doesn't have any pfnInputShape variables
    if( pfnInputShape.first == 0 and pfnInputShape.second == 0 ){
        isPFN = false;
        loadKerasModel("xgboostModel", model_file_name );
    }else {
        isPFN = true;
	    loadKerasModel("kerasModel", model_file_name );
    }
}


PFNReader::PFNReader( const PFNReader& rhs) :
	pfnShape( rhs.pfnShape ),
	highlevelShape( rhs.highlevelShape ),
	kerasModel( rhs.kerasModel ),
	predictRoutine( rhs.predictRoutine )
{
	++instanceCounter;
}


PFNReader::~PFNReader(){
    --instanceCounter;
}


void PFNReader::initializePythonAPI(){
	Py_Initialize();
	PyObject* sysPath = PySys_GetObject( "path" );
    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("/"));
    PyList_Insert( sysPath, 0, PyUnicode_FromString( dir_path.c_str() ));
    PyList_Insert( sysPath, 0, PyUnicode_FromString("./"));
}


void PFNReader::loadPythonModule( const std::string& module_name ){
	try{
		pythonModule = boost::python::import( module_name.c_str() );
	} catch( ... ){
		PyErr_Print();
	}
}


void PFNReader::loadKerasModel( const char * model_type, const std::string& model_name ){
	try{
		kerasModel =  pythonModule.attr( model_type )( model_name );
        predictRoutine = kerasModel.attr("predict");
	} catch( ... ){
		PyErr_Print();
	}
}


template < typename T > boost::python::list vectorToPythonList( const std::vector< T >& vec ){
    boost::python::list pyList;
    for(const T& entry : vec ){
        pyList.append( entry );
    }
    return pyList;
}


bool PFNReader::checkPfnShape( const std::vector< std::vector< double > >& vec) const{
    if( vec.size() != pfnShape.first ) return false;
    if( vec.size() != 0 && vec[0].size() != pfnShape.second ) return false;
    return true;
}


bool PFNReader::checkHighlevelShape( const std::vector< double >& vec) const{
    return ( vec.size() == highlevelShape );
}


double PFNReader::predict( const std::vector< double >& highlevelInput, const std::vector< std::vector< double > >& pfnInput ) const{
    if( isPFN ) return predictPFN( highlevelInput, pfnInput );
    else return predictBDT( highlevelInput );
}


double PFNReader::predictPFN( const std::vector< double >& highlevelInput, const std::vector< std::vector< double > >& pfnInput ) const{
    if( !checkPfnShape( pfnInput ) ){
        throw std::invalid_argument( "PFN input vector has wrong shape. Shape is supposed to be (" + std::to_string( pfnShape.first ) + ", " + std::to_string( pfnShape.second ) + ")." );
    }
    if( !checkHighlevelShape( highlevelInput ) ){
        throw std::invalid_argument( "High level input vector has wrong shape. Shape is supposed to be (" + std::to_string( highlevelShape ) + ")." );
    }
    std::vector< boost::python::list > pfn_list_vector;
    for( const auto& vec : pfnInput ){
        pfn_list_vector.push_back( vectorToPythonList( vec ) );
    }
    boost::python::list pfn_list = vectorToPythonList( pfn_list_vector );
    boost::python::list highlevel_list = vectorToPythonList( highlevelInput );

    boost::python::object pythonPrediction = predictRoutine( highlevel_list, pfn_list );
    return boost::python::extract< double >( pythonPrediction );
}


double PFNReader::predictBDT( const std::vector< double >& highlevelInput ) const{
    if( !checkHighlevelShape( highlevelInput ) ){
        throw std::invalid_argument( "BDT input vector has wrong shape. Shape is supposed to be (" + std::to_string( highlevelShape ) + ")." );
    }
    boost::python::list highlevel_list = vectorToPythonList( highlevelInput );
    boost::python::object pythonPrediction = predictRoutine( highlevel_list );
    return boost::python::extract< double >( pythonPrediction );
}
