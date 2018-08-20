#include <numpy/arrayobject.h>
#include <Python.h>

static PyObject *notify(PyObject *self, PyObject *args) {
  PyObject *py_object;
  uint16_t size;

  if (!PyArg_ParseTuple(args, "O", &py_object)) {
    return NULL;
  }
  PyObject *data_py_object =
      PyArray_FROM_OTF(py_object, NPY_UBYTE, NPY_IN_ARRAY);
  if (data_py_object == NULL) {
    Py_XDECREF(data_py_object);
    return NULL;
  }

  size = (uint16_t)PyArray_DIM(data_py_object, 0);
  uint8_t *data = (uint8_t *)PyArray_DATA(data_py_object);
  { // calls C fundcion to send data
    printf("Sending...\n");
    for (int i = 0; i < size; i++)
      printf("%d", data[i]);
  }

  Py_XDECREF(data_py_object);
  Py_RETURN_NONE;
}

static PyMethodDef ble_serial_methods[] = {
    {"notify", notify, METH_VARARGS, "Sends a notification."},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC init_ble_serial_module(void) {
  if (!Py_InitModule3("ble_serial_module", ble_serial_methods)) {
    return NULL;
  }
  import_array();
}