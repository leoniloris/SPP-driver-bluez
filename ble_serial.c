#include <Python.h>
#include <numpy/arrayobject.h>

enum Return_t {
  MEMORY_ERROR = -1,
  SUCCESS
};

static char module_docstring[] =
    "This module provides an interface for writing and reading through a "
    "serial->BLE interface.";

static PyObject *notify(PyObject *self, PyObject *args) {
  PyObject *py_object;
  uint16_t size;

  if (!PyArg_ParseTuple(args, "O", &py_object)) {
    return NULL;
  }
  size = PyObject_Length(py_object);
  if (size > 255) {
    printf("Invalid size.\n");
    return Py_BuildValue("i", MEMORY_ERROR);
  }

  uint8_t *data = (uint8_t*) malloc(size);
  if (!data) {
    printf("Could not allocate memory for notification.\n");
    return NULL;
  }

  for (uint8_t i = 0; i < size; i++) {
      PyLongObject *item = PyList_GetItem(py_object, i);
      data[i] = (uint8_t)PyLong_AsLong(item);
  }

  { // calls C fundcion to send data
    printf("Sending...\n");
    for (int i = 0; i < size; i++)
      printf("%d", data[i]);
  }
  free(data);
  return Py_BuildValue("i", SUCCESS);
}

static PyMethodDef ble_serial_methods[] = {
    {"notify", notify, METH_VARARGS, "Sends a notification."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef ble_serial_module = {
    PyModuleDef_HEAD_INIT, "ble_serial", /* name of module */
    module_docstring, /* module documentation, may be NULL */
    -1,               /* size of per-interpreter state of the module,
                         or -1 if the module keeps state in global variables. */
    ble_serial_methods};

PyMODINIT_FUNC PyInit_ble_serial(void) {
  return PyModule_Create(&ble_serial_module);
}