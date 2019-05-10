#include "PFNReader.h"

//include c++ library classes
#include <stdexcept>
#include <iostream>

unsigned PFNReader::instanceCounter = 0;
boost::python::object PFNReader::pythonModule = boost::python::object();


PFNReader::PFNReader( const std::string& model_file_name, const std::pair<unsigned, unsigned>& pfnInputShape, const unsigned highlevelInputShape ):
    pfnShape( pfnInputShape ), highlevelShape( highlevelInputShape )
{
    if( instanceCounter == 0 ){
		initializePythonAPI();
		loadPythonModule( "kerasPredict" );
    }
    ++instanceCounter;
	loadKerasModel( model_file_name );
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
	PyList_Insert( sysPath, 0, PyUnicode_FromString("./"));
}


void PFNReader::loadPythonModule( const std::string& module_name ){
	try{
		pythonModule = boost::python::import( module_name.c_str() );
	} catch( ... ){
		PyErr_Print();
	}
}


void PFNReader::loadKerasModel( const std::string& model_name ){
	try{
		kerasModel =  pythonModule.attr("kerasModel")(model_name);
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


double PFNReader::predict( const std::vector< std::vector< double > >& pfnInput, const std::vector< double >& highlevelInput ) const{
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

    boost::python::object pythonPrediction = predictRoutine( pfn_list, highlevel_list );
    return boost::python::extract< double >( pythonPrediction );
}
